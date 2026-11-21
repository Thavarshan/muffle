# Platforms

Muffle is intended for macOS and Linux systems providing POSIX process APIs.
The repository's current CI build matrix runs GCC and Clang on Ubuntu and
Apple Clang on macOS. This is the tested CI matrix, not a claim that every
system or compiler combination is supported.

The implementation depends on `fork`, `execvp`, `pipe`, `dup2`, `waitpid`,
process groups, POSIX signals, and POSIX file operations. CMake rejects
Windows builds; native Windows support is not implemented.

## Linux release binary requirements

The x86_64 and arm64 Linux release archives are built and executed on Ubuntu
22.04 runners. Inspection of those binaries shows requirements for glibc
2.34 or newer and a `libstdc++` that provides `GLIBCXX_3.4.29` or newer.
They are dynamically linked and are not universal or fully static binaries.
The Debian package metadata records the runtime dependencies detected by
`dpkg-shlibdeps`; RPM metadata is generated from the linked requirements.
Linux distributions older than the build environment are not claimed as
supported without a successful installation and execution test.

Programs requiring an interactive terminal, terminal control, separate
stdout and stderr streams, or reliable terminal signal/job-control behavior
are outside scope. Muffle combines output into a pipe and does not allocate a
pseudo-terminal. See [limitations](limitations.md).
