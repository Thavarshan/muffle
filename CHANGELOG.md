# Changelog

Notable changes to Muffle are recorded here. The project follows Semantic
Versioning while pre-1.0; CLI behavior may change between minor releases.

## [0.1.0] - 2026-11-21

- First public release of the native C++17 POSIX command wrapper.
- Quiet success summaries, configurable failure tails, verbose streaming,
  persistent logs, and child status propagation.
- Improved SIGPIPE handling for closed verbose output destinations.
- Forward SIGINT/SIGTERM and escalate after a 500 ms grace interval when a
  child does not exit.
- Install the `muffle(1)` manual page and CPack source packages.
- Add release automation for macOS arm64/x86_64 and Linux arm64/x86_64
  archives, Debian packages, RPM packages, checksums, and provenance.
- Add extended integration coverage for file/stream failures, binary output,
  large output, and child cleanup.
