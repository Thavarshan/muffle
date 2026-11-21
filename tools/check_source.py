#!/usr/bin/env python3
"""Enforce repository text, width and comment hygiene rules."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
EXCLUDED = {"build", ".git", ".cache", "__pycache__"}
EXTENSIONS = {".cpp", ".hpp", ".py", ".sh", ".yml", ".yaml", ".md"}
NAMES = {"CMakeLists.txt", "Doxyfile", ".clang-tidy", ".clang-format",
         ".editorconfig", ".gitignore", ".gitattributes"}
MARKERS = re.compile(r"\b(?:TODO|FIXME|HACK)\b(?![^\n]*\b#\d+\b)")
COMMENTED_CODE = re.compile(
    r"^\s*//\s*(?:return|if|for|while|auto|int|std::)\b"
)


def inspect(path):
    """Report violations in one UTF-8 repository text file."""
    problems = []
    data = path.read_bytes()
    relative = path.relative_to(ROOT)
    try:
        content = data.decode("utf-8")
    except UnicodeDecodeError:
        return [f"{relative}: file must be UTF-8"]
    if data and not data.endswith(b"\n"):
        problems.append(f"{relative}: missing final newline")
    for number, line in enumerate(content.splitlines(), 1):
        prefix = f"{relative}:{number}"
        if len(line) > 80:
            problems.append(f"{prefix}: line exceeds 80 characters")
        if line.rstrip() != line:
            problems.append(f"{prefix}: trailing whitespace")
        if "\t" in line:
            problems.append(f"{prefix}: tabs are prohibited")
        if "\r" in line:
            problems.append(f"{prefix}: CR characters are prohibited")
        if path.suffix in {".cpp", ".hpp"}:
            if MARKERS.search(line):
                problems.append(f"{prefix}: marker needs a GitHub issue #N")
            if COMMENTED_CODE.search(line):
                problems.append(f"{prefix}: commented-out code is prohibited")
            if line.strip() == "//":
                problems.append(f"{prefix}: empty comment is prohibited")
    return problems


def main():
    """Check all project-owned text files and return a CI exit status."""
    problems = []
    for path in ROOT.rglob("*"):
        if not path.is_file():
            continue
        parts = path.relative_to(ROOT).parts
        if any(part in EXCLUDED or part.startswith("build-")
               for part in parts):
            continue
        if path.suffix not in EXTENSIONS and path.name not in NAMES:
            continue
        problems.extend(inspect(path))
    if problems:
        print("\n".join(sorted(problems)), file=sys.stderr)
        return 1
    print("Source hygiene and 80-column checks passed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
