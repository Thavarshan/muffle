# FAQ

## Does Muffle run commands through a shell?

No. It calls `execvp` with separate arguments. Invoke `sh -c` explicitly if
you need shell syntax.

## Can I see output while the command runs?

Yes. `--verbose` streams captured bytes to Muffle's stdout and still records
them in the log.

## Can I keep output when a command succeeds?

Yes. Use `--log PATH` with a path that does not already exist. Otherwise the
temporary log is removed on success.

## Why are stdout and stderr combined?

The current implementation redirects both child streams to one pipe and one
log. It does not retain their separate identities.

## Can Muffle run interactive programs?

Not reliably. It does not allocate a pseudo-terminal or preserve terminal
control behavior.

## Does Muffle support Windows?

No. The current runner uses POSIX process APIs and CMake rejects Windows.

## Does it preserve the command's exit code?

Normal child exit codes are returned unchanged. Signal termination maps to
128 plus the signal number. Muffle reserves 125 for internal errors, 126 and
127 for execution errors, and 2 for invalid CLI arguments.

## Does Muffle redact secrets from logs?

No. Log files are created with restrictive permissions, but output is not
redacted or encrypted. Treat logs as sensitive.
