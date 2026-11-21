# Testing

## Test suites

CTest runs two tests:

- `unit.cli` is a C++ executable testing parser actions, option values,
  token preservation, and invalid arguments.
- `integration.cli` is a Python `unittest` suite that launches the compiled
  executable and checks help/version, suppression, failure tails, combined
  streams, verbose behavior, persistent log permissions and overwrite
  refusal, argument safety, missing executables, partial lines, bounded
  tails, and SIGTERM forwarding.

## Run tests

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

To instrument with AddressSanitizer and UndefinedBehaviorSanitizer on GCC or
Clang:

```sh
cmake -S . -B build-sanitize \
  -DCMAKE_BUILD_TYPE=Debug \
  -DMUFFLE_ENABLE_SANITIZERS=ON
cmake --build build-sanitize --parallel 2
ctest --test-dir build-sanitize --output-on-failure
```

## Adding tests

Add parser cases to `tests/unit_cli.cpp`. For process behavior, extend
`tests/integration.py` with an isolated temporary directory and assert the
actual status and streams. Avoid timing-sensitive assertions except where
signal behavior requires them; use a bounded timeout and clean up processes.
Do not make tests depend on network access or optional tools unless the test
explicitly skips when unavailable.

## Continuous integration

`.github/workflows/ci.yml` runs the strict quality script on Ubuntu, builds
and tests with GCC and Clang on Ubuntu and Apple Clang on macOS, and runs a
GCC/Clang sanitizer-enabled Linux job. Pull requests, pushes, and manual
workflow dispatch trigger CI. The project requires no external test service.

## Common failures

- A CTest integration failure often includes the captured child status and
  output; reproduce the exact command directly to isolate it.
- Sanitizer failures can point to undefined behavior or memory errors; fix
  the source issue rather than disabling instrumentation.
- `scripts/check.sh` exits early if a required quality tool is missing.
  Install the named tool and rerun the complete command.
