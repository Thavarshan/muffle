# Troubleshooting

## CLI and child execution

- **Missing `--` or command:** Run `muffle --help`, then add
  `-- COMMAND [ARG ...]`.
- **Status 127:** `execvp` could not find the executable or interpreter.
  Check spelling, PATH, and `command -v NAME` in the same environment.
- **Status 126:** The executable could not run. Check its permissions, file
  format, and interpreter path.
- **A pipeline or `&&` does not work:** Muffle passes arguments directly.
  Use `muffle -- sh -c '...'` for explicit shell interpretation.
- **Interactive program behaves incorrectly:** The child has a pipe, not a
  terminal. Run it directly; interactive TTY workflows are unsupported.

## Logs and output

- **Status 125 before child output:** Muffle could not create or use a log,
  pipe, or process. Check stderr, temporary-directory permissions, free disk
  space, and the requested path.
- **`--log` fails for an unused path:** The destination may exist as a file
  or symlink. Check with `ls -la PATH`; choose another path or remove the old
  file deliberately.
- **Successful output is not visible:** Quiet mode is the default. Add
  `--verbose` to stream output or `--log PATH` to retain it.
- **Log is missing after success:** Temporary logs are deleted on success.
  Add `--log PATH` to retain successful output.
- **Too much failure context:** The default tail is 40 lines. Re-run with
  `--tail N` or inspect the full retained log.

## Installation and tests

- **`muffle` is not found after install:** Check `command -v muffle`, run the
  installed binary by absolute path, and add the prefix's `bin` directory to
  PATH if needed.
- **A check tool is missing:** `scripts/check.sh` requires Python 3, PyYAML,
  CMake, CTest, clang-format, clang-tidy, and Doxygen. Install the named tool
  and rerun the complete gate.

When reporting a problem, include version, OS, reproduction, actual status,
and sanitized output. Never publish credentials or private data from logs.
