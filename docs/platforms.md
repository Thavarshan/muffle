# Platforms

Muffle is intended for macOS and Linux systems providing POSIX process APIs.
The repository's current CI build matrix runs GCC and Clang on Ubuntu and
Apple Clang on macOS. This is the tested CI matrix, not a claim that every
system or compiler combination is supported.

The implementation depends on `fork`, `execvp`, `pipe`, `dup2`, `waitpid`,
process groups, POSIX signals, and POSIX file operations. CMake rejects
Windows builds; native Windows support is not implemented.

Programs requiring an interactive terminal, terminal control, separate
stdout and stderr streams, or reliable terminal signal/job-control behavior
are outside scope. Muffle combines output into a pipe and does not allocate a
pseudo-terminal. See [limitations](limitations.md).
