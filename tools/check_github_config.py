#!/usr/bin/env python3
"""Check GitHub YAML syntax and required issue form files."""

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent.parent
REQUIRED = (
    ".github/ISSUE_TEMPLATE/bug_report.yml",
    ".github/ISSUE_TEMPLATE/feature_request.yml",
    ".github/ISSUE_TEMPLATE/documentation.yml",
    ".github/ISSUE_TEMPLATE/config.yml",
)


def main():
    try:
        import yaml
    except ImportError:
        print("Missing PyYAML; install requirements-dev.txt", file=sys.stderr)
        return 1

    files = [ROOT / name for name in REQUIRED]
    files.extend(sorted((ROOT / ".github").glob("**/*.yml")))
    errors = []
    for path in files:
        if not path.is_file():
            errors.append(f"missing required file: {path.relative_to(ROOT)}")
            continue
        try:
            with path.open(encoding="utf-8") as stream:
                yaml.safe_load(stream)
        except (yaml.YAMLError, OSError) as error:
            errors.append(f"{path.relative_to(ROOT)}: {error}")
    if errors:
        for error in errors:
            print(error, file=sys.stderr)
        return 1
    print(f"GitHub YAML check passed ({len(files)} file checks)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
