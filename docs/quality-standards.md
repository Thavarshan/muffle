# Quality standards

The existing project configurations and scripts are authoritative:
`.editorconfig`, `.clang-format`, `.clang-tidy`, `CMakeLists.txt`, and
`scripts/check.sh`.

- C++17, four-space C++ indentation, and an 80-column source limit.
- CMake uses four spaces; YAML uses two spaces.
- `clang-format` must pass in dry-run mode.
- Compiler warnings are errors (`-Wall`, `-Wextra`, `-Wpedantic`,
  `-Wconversion`, `-Wsign-conversion`, `-Wshadow`, and `-Wformat=2`).
- `clang-tidy` runs configured analyzer, bug-prone, portability, performance,
  and selected readability/modernization checks; configured warnings are
  errors.
- Public API headers use Doxygen. Doxygen warnings fail the gate.
- Source hygiene checks include whitespace and comment rules; Python files
  are syntax-compiled and shell scripts are checked with `bash -n`.
- Unit and integration tests run under CTest.

Run the full gate with `bash scripts/check.sh`. Do not relax checks merely to
make a change pass; correct the source, tests, or documentation instead.
