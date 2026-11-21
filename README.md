# Muffle

**Less noise. More signal.**

Muffle is a tiny C++17 CLI that runs noisy development commands while
suppressing successful output and keeping failures debuggable. No language
runtime or third-party C++ dependencies are required.

## Quick start

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/muffle -- npm test
./build/muffle --tail 100 -- cmake --build build
./build/muffle --verbose -- cargo test
./build/muffle --log build.log -- make
```

Use `--` to separate Muffle options from the child command and arguments.
Commands are executed directly using `execvp`, not via a shell.
For shell pipelines, explicitly invoke a shell:

```sh
./build/muffle -- sh -c 'printf "hello\\n" | cat'
```

On success, Muffle prints an elapsed-time summary and discards its temporary
log. On failure, it prints the exit status, final 40 lines and private log
path. `--tail N` changes the line limit; `--verbose` streams the combined
output as it arrives. `--log PATH` saves a persistent log on success too,
but will not overwrite an existing file. Logs are initially mode `0600`.

Exit codes are preserved. A child terminated by signal `N` returns `128+N`.
Missing executables return 127; other execution failures return 126.
Muffle's own errors return 125, and invalid CLI arguments return 2.

**Platform support:** macOS and Linux (POSIX process APIs). Windows is not
yet supported. Child stdout and stderr are intentionally combined. Commands
requiring an interactive TTY or separate output streams are not supported.

## Development

Requirements: CMake 3.20+, a C++17 compiler and Python 3. For complete
quality gates, install `clang-format`, `clang-tidy` and `doxygen`.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
bash scripts/check.sh
bash scripts/setup-hooks.sh
```

`check.sh` enforces an 80-character line limit, whitespace and comment
hygiene, strict compiler diagnostics, clang-format, clang-tidy, Doxygen
warnings as errors, and both unit and integration tests. CI also checks
GCC, Clang, macOS, and address/undefined-behavior sanitizers. Hook
installation is per clone via `core.hooksPath`.

## Design

- `include/muffle/cli.hpp` and `src/cli.cpp`: argument parser.
- `include/muffle/runner.hpp` and `src/runner.cpp`: process execution.
- `tests/unit_cli.cpp`: parser unit tests.
- `tests/integration.py`: actual subprocess behavior tests.
- `tools/check_source.py`: documentation-adjacent source hygiene rules.

The runner uses a private log to avoid unbounded in-memory output buffers.
A reverse scan displays only the final requested number of lines. Signals
are forwarded to the child process group. No telemetry or AI APIs are used.

See [CONTRIBUTING.md](CONTRIBUTING.md) for contributor expectations.
