#!/usr/bin/env bash
set -euo pipefail

# Demonstrate quiet success and actionable failures without build tools.
root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
binary="${1:-$root/build/muffle}"

if [[ ! -x "$binary" ]]; then
    printf 'Muffle executable not found: %s\n' "$binary" >&2
    printf 'Build first: cmake -S . -B build && cmake --build build\n' >&2
    exit 1
fi

demo_tmp="$(mktemp -d)"
trap 'rm -rf "$demo_tmp"' EXIT
export TMPDIR="$demo_tmp"

printf '\n%s\n' '=== Quiet success ==='
"$binary" -- sh -c 'printf "Passing tests (hidden)\n"'

printf '\n%s\n' '=== Useful failure tail (simulated) ==='
if "$binary" --tail 2 -- sh -c '
    printf "Compiling module A...\n"
    printf "Compiling module B...\n"
    printf "error: simulated test failure\n"
    exit 42
'; then
    printf 'Expected the simulated failure to return 42.\n' >&2
    exit 1
else
    status="$?"
fi

if [[ "$status" -ne 42 ]]; then
    printf 'Unexpected exit status: %s (wanted 42).\n' "$status" >&2
    exit 1
fi

printf '\nMuffle correctly preserved the child exit code: %s\n' "$status"
printf '%s\n' 'Demo complete; its temporary logs have been removed.'
