#!/usr/bin/env python3
"""Validate local Markdown links and core documentation structure."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parent.parent
MARKDOWN = sorted(ROOT.rglob("*.md"))
EXCLUDED = {"build"}
REQUIRED = {
    "README.md",
    "CONTRIBUTING.md",
    "CODE_OF_CONDUCT.md",
    "SECURITY.md",
    "SUPPORT.md",
    "GOVERNANCE.md",
    "CHANGELOG.md",
    "docs/index.md",
    "docs/cli-reference.md",
    "docs/exit-codes.md",
    "docs/logging.md",
    "docs/architecture.md",
    "docs/testing.md",
    "docs/limitations.md",
    "docs/roadmap.md",
    "docs/release-notes/v0.1.2.md",
    "docs/maintainers/packaging.md",
}
LINK = re.compile(r"(?<!!)\[[^\]]+\]\(([^)]+)\)")
HEADING = re.compile(r"^#{1,6}\s+(.+?)\s*#*\s*$")
FENCE = re.compile(r"^\s*(`{3,}|~{3,})(.*)$")


def markdown_files():
    return [path for path in MARKDOWN if not EXCLUDED.intersection(path.parts)]


def anchors(source):
    """Return GitHub-style slugs for Markdown headings."""
    result = set()
    counts = {}
    for line in source.splitlines():
        match = HEADING.match(line)
        if not match:
            continue
        heading = re.sub(r"`([^`]*)`", r"\1", match.group(1)).lower()
        heading = re.sub(r"[^\w -]", "", heading)
        slug = re.sub(r"\s+", "-", heading.strip())
        count = counts.get(slug, 0)
        counts[slug] = count + 1
        result.add(slug if count == 0 else f"{slug}-{count}")
    return result


def check_links(paths):
    errors = []
    for path in paths:
        source = path.read_text(encoding="utf-8")
        for number, line in enumerate(source.splitlines(), start=1):
            for target in LINK.findall(line):
                if target.startswith(("https://", "http://", "mailto:", "#")):
                    continue
                local, separator, fragment = target.partition("#")
                candidate = (path.parent / local).resolve() if local else path
                if not candidate.exists():
                    message = f"{path.relative_to(ROOT)}:{number}: {target}"
                    errors.append(message)
                elif separator and fragment and candidate.suffix == ".md":
                    target_source = candidate.read_text(encoding="utf-8")
                    if fragment not in anchors(target_source):
                        errors.append(
                            f"{path.relative_to(ROOT)}:{number}: "
                            f"missing anchor {target}"
                        )
    return errors


def check_markdown(paths):
    errors = []
    for path in paths:
        fence = None
        for number, line in enumerate(
            path.read_text(encoding="utf-8").splitlines(), start=1
        ):
            match = FENCE.match(line)
            if match:
                marker = match.group(1)[0]
                if fence is None:
                    fence = marker
                    if not match.group(2).strip():
                        errors.append(
                            f"{path.relative_to(ROOT)}:{number}: "
                            "code fence needs a language identifier"
                        )
                elif marker == fence:
                    fence = None
        if fence is not None:
            errors.append(f"{path.relative_to(ROOT)}: unclosed code fence")
    return errors


def main():
    paths = markdown_files()
    missing = sorted(name for name in REQUIRED if not (ROOT / name).is_file())
    errors = [f"missing required documentation: {name}" for name in missing]
    errors.extend(check_links(paths))
    errors.extend(check_markdown(paths))
    if errors:
        print("Documentation check failed:", file=sys.stderr)
        for error in errors:
            print(f"  {error}", file=sys.stderr)
        return 1
    print(f"Documentation check passed ({len(paths)} Markdown files)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
