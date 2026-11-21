# Releasing

Muffle has version `0.1.0` in `project(muffle VERSION ...)` and reports that
compiled version with `--version`. No Git tags, GitHub releases, packages,
or prebuilt binaries are currently published.

Before a release, maintainers should:

1. Review user-visible changes and update `CHANGELOG.md`.
2. Run `bash scripts/check.sh` and inspect all CI jobs on the target commit.
3. Build and run CTest with the supported CI compiler/platform matrix.
4. Review versioning implications for CLI flags, output, and exit statuses.
5. Update the CMake project version and verify `muffle --version`.
6. Create an annotated version tag only after release readiness is confirmed.
7. Publish release notes describing changes, limitations, and source links.

Packaging and binary distribution are not currently configured. Do not imply
that a package manager or download channel is supported until it exists.
