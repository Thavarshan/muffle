#!/usr/bin/env bash
set -euo pipefail

binary="${1:?usage: package-smoke-test.sh BINARY VERSION}"
version="${2:?usage: package-smoke-test.sh BINARY VERSION}"
temporary="$(mktemp -d)"
trap 'rm -rf -- "$temporary"' EXIT

actual_version="$("$binary" --version)"
test "$actual_version" = "muffle $version"
quiet_output="$(TMPDIR="$temporary" "$binary" -- sh -c 'echo success')"
case "$quiet_output" in
    'muffle: passed ('*) ;;
    *) echo "Unexpected successful output: $quiet_output" >&2; exit 1 ;;
esac
case "$quiet_output" in
    *success*) echo "Successful child output was not suppressed" >&2; exit 1 ;;
esac

after="$(TMPDIR="$temporary" "$binary" -- sh -c \
    'echo expected-failure; exit 42' 2>&1)" || status=$?
status="${status:-0}"
test "$status" -eq 42
case "$after" in
    *expected-failure*'Log: '*) ;;
    *) echo "Missing failure tail/log path: $after" >&2; exit 1 ;;
esac
log_path="$(printf '%s\n' "$after" | sed -n 's/^Log: //p')"
test -f "$log_path"
mode="$(python3 -c '
import os
import sys
print(oct(os.stat(sys.argv[1]).st_mode & 0o777)[2:])
' "$log_path")"
test "$mode" = "600"
