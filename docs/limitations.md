# Limitations

The following are properties of the current implementation, not promised
future behavior:

- Native Windows builds are unsupported; CMake requires POSIX process APIs.
- Interactive TTY workflows and pseudo-terminal allocation are unsupported.
- Child stdout and stderr are combined; stream identity and exact independent
  stream order are not preserved.
- Muffle captures output to disk, so large output requires available disk
  space. There is no size limit or rotation policy.
- Logs are not redacted or encrypted. File permissions do not protect against
  the invoking user or privileged processes.
- SIGINT and SIGTERM forwarding and process cleanup are best-effort. A child
  that creates its own process group or leaves descendants may outlive Muffle.
- There is no configuration file, plugin system, telemetry, or published
  package/binary distribution.
