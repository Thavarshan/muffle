# Development

Muffle uses C++17 and CMake. Its source is split between the CLI parser in
`src/cli.cpp`, the executable entry point in `src/main.cpp`, and process
execution in `src/runner.cpp`. Public headers live in `include/muffle/`.

## Configure and build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

For sanitizer instrumentation with GCC or Clang:

```sh
cmake -S . -B build-sanitize \
  -DCMAKE_BUILD_TYPE=Debug \
  -DMUFFLE_ENABLE_SANITIZERS=ON
cmake --build build-sanitize --parallel 2
ctest --test-dir build-sanitize --output-on-failure
```

Install the pinned documentation validator in a virtual environment before
running the complete quality command:

```sh
python3 -m venv .venv
. .venv/bin/activate
python -m pip install -r requirements-dev.txt
bash scripts/check.sh
```

The gate requires Python 3, PyYAML, CMake, CTest, clang-format, clang-tidy,
and Doxygen. The optional local hook is installed with
`bash scripts/setup-hooks.sh`.

See [testing](testing.md), [quality standards](quality-standards.md), and
[CONTRIBUTING.md](../CONTRIBUTING.md) before changing behavior.
