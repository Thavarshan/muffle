# Contributing to Muffle

Thanks for considering a contribution. Focused bug fixes, tests, and
corrections to the documentation are useful. Please report a bug or propose
substantial behavior changes in GitHub Issues before investing in a large
change.

## Prerequisites

- macOS or Linux with POSIX process APIs
- CMake 3.20 or newer
- A C++17 compiler (GCC, Clang, or Apple Clang)
- Python 3
- `clang-format`, `clang-tidy`, and Doxygen for the complete quality gate

## Set up a checkout

```sh
git clone https://github.com/Thavarshan/muffle.git
cd muffle
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

For the full local gate, install the analysis tools and the pinned docs
validator in a virtual environment, then run `bash scripts/check.sh`:

```sh
python3 -m venv .venv
. .venv/bin/activate
python -m pip install -r requirements-dev.txt
```

The optional pre-commit hook setup is provided by
`bash scripts/setup-hooks.sh`; this configures this clone to run the same
gate before each commit.

## Project layout

- `src/main.cpp` connects parsed actions to the process runner.
- `src/cli.cpp` parses options and builds help text.
- `src/runner.cpp` creates the capture/log files, launches and waits for the
  child, forwards selected signals, and reports status and output.
- `include/muffle/` contains the documented public interfaces.
- `tests/unit_cli.cpp` tests parser behavior.
- `tests/integration.py` runs the executable and checks process behavior.
- `docs/` contains user, contributor, and maintainer documentation.

## Coding and documentation standards

- Write C++17 and retain POSIX scope unless a proposal explicitly changes it.
- Use four spaces for C++, CMake, and general project files; YAML uses two.
- Keep source lines at or below 80 columns and format C++ with clang-format.
- Compiler warnings are errors. Do not weaken the warning, analyzer,
  sanitizer, or documentation gates to accommodate a change.
- Document public header contracts with Doxygen, including parameters and
  return values. Keep comments useful and current; do not commit commented-
  out code. TODO/FIXME/HACK comments must reference a GitHub issue.
- Avoid dependencies unless a clear maintenance or user benefit justifies
  them. Do not add runtime dependencies for documentation or tests.
- Keep CLI behavior and output claims consistent across help, tests, and docs.

## Tests and changes to process behavior

Run `bash scripts/check.sh` before submitting. A change to process execution
should include focused unit coverage where practical and integration tests
that assert actual status, stdout/stderr behavior, logging, and signals as
relevant. Tests should be deterministic, use temporary directories, and avoid
network access. See [testing](docs/testing.md) and [quality
standards](docs/quality-standards.md).

## Bug reports and feature suggestions

Use the matching issue form. Include a minimal reproduction, the Muffle
version, operating system, relevant compiler/build details, observed exit
code, and expected versus actual behavior. Sanitize logs and remove secrets,
private paths, and personal data. Feature proposals should explain the
underlying use case and alternatives. Suggestions are reviewed by the
maintainer for fit, scope, support cost, and compatibility; submitting an
idea does not guarantee it will be implemented.

## Commits and review

Use short, imperative commit subjects with a clear area, for example
`docs: add logging reference` or `fix: handle interrupted capture`. Keep
commits focused and explain non-obvious decisions in the body. Maintainer
routine work is committed directly to `main`; external contributors may
submit pull requests. External changes are reviewed for correctness,
maintainability, tests, docs, and compatibility. A change can be requested,
revised, or declined when it conflicts with project scope or support capacity.

Preserve compatibility where practical. Changes to options, output, or exit
status are user-visible and require explicit documentation and tests. The
maintainer makes final decisions about scope and releases under the
single-maintainer model described in [GOVERNANCE.md](GOVERNANCE.md).
