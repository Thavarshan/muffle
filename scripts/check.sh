#!/usr/bin/env bash
set -euo pipefail

for tool in python3 cmake ctest clang-format clang-tidy doxygen; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        echo "Missing required check tool: $tool" >&2
        exit 1
    fi
done

if ! python3 -c 'import yaml' >/dev/null 2>&1; then
    echo "Missing PyYAML; install requirements-dev.txt in a venv" >&2
    exit 1
fi

python3 tools/check_source.py
python3 tools/check_docs.py
python3 tools/check_github_config.py
python3 -m py_compile tests/integration.py tools/check_source.py
bash -n scripts/*.sh .githooks/pre-commit
clang-format --dry-run --Werror src/*.cpp include/muffle/*.hpp \
    tests/*.cpp

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure

for source in src/*.cpp tests/*.cpp; do
    clang-tidy -p build "$source"
done

doxygen Doxyfile
