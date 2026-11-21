# Muffle

**Less noise. More signal.**

Muffle runs a command and keeps its successful output out of your terminal.
When the command fails, it shows the final lines of captured output and keeps
its log available for diagnosis. The child process's exit status is returned
so scripts and CI can react to the same result.

Build tools, tests, package managers, and CI jobs can produce hundreds of
routine lines before reaching the one error that needs attention. Muffle
keeps those lines in a log and puts a concise success or failure summary in
the foreground. This can also make AI-assisted development easier to review
by reducing routine terminal output; Muffle makes no token or productivity
claims.

Use it for local development, build scripts, test runs, and CI steps. Muffle
is a native C++17 executable using POSIX process APIs. It has no third-party
C++ or runtime dependency.

[![CI][ci]][ci-workflow]
[![MIT License][license-badge]](LICENSE)
[![Latest release][release-badge]](releases)
![C++17][cpp]
![macOS and Linux][platform]

## Features and scope

| Capability | Current behavior |
| --- | --- |
| Quiet execution | Suppresses captured child output on success by default. |
| Failure output | Up to the final 40 lines plus a failure summary. |
| Tail length | `--tail N` sets the positive number of lines shown on failure. |
| Persistent log | Retains output; refuses existing paths; requests `0600`. |
| Temporary log | Removed after success and retained after failure. |
| Verbose mode | Streams captured bytes while recording them. |
| Exit status | Returns child codes; signals map to `128 + signal`. |
| Signals | Forwards SIGINT and SIGTERM to the child group while it is active. |
| Arguments | Executes directly through `execvp`; no implicit shell parsing. |
| Output streams | stdout and stderr are combined into one pipe and one log. |
| Platforms | macOS/Linux intended; Windows and interactive TTYs unsupported. |
| Runtime | Native executable; no third-party runtime dependency. |

## Quick start

Install the verified Homebrew formula on macOS or Linux:

```sh
brew install Thavarshan/muffle/muffle
muffle --version
muffle -- npm test
```

Homebrew 7 may ask you to review and trust the third-party formula. For a
source build, use CMake 3.20+, a C++17 compiler, and Python 3 for tests:

```sh
git clone https://github.com/Thavarshan/muffle.git
cd muffle
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
./build/muffle -- npm test
```

For the complete local quality gate, create and activate a Python virtual
environment, install `requirements-dev.txt`, and run `bash scripts/check.sh`.

On macOS, install the command line developer tools and CMake. On Ubuntu or
Debian, install `build-essential`, `cmake`, and `python3`. See
[installation](docs/installation.md) for build variants, install steps, and
platform details.

## Try the demo

After building, run a self-contained demonstration. It needs only a POSIX
shell and Muffle: no npm, Docker, or Cargo installation.

```sh
bash scripts/demo.sh
```

It shows quiet success and a simulated failure with a two-line tail,
verifies that exit code 42 is preserved, then cleans up temporary logs.

## What it looks like

Illustrative output (elapsed time and temporary path vary):

```text
$ muffle -- make test
muffle: passed (2.31s)

$ muffle --tail 3 -- sh -c 'printf "setup\\nerror\\n"; exit 1'
muffle: failed (exit 1, 0.01s)
setup
error
Log: /tmp/muffle-a1b2c3
```

The failure output and log path go to stderr. The success summary goes to
stdout. `--verbose` streams captured output to stdout while running, then
prints the success summary there. Since stdout and stderr are merged, verbose
output does not preserve their original stream distinction or exact
cross-stream write order.

## Practical examples

Muffle does not require these tools; they are examples of commands it can
run when installed in your environment.

```sh
muffle -- npm test
muffle --tail 80 -- npm run build
muffle -- composer install
muffle -- ./vendor/bin/phpunit
muffle -- cmake --build build --parallel 2
muffle -- make -j2
muffle -- cargo test
muffle -- docker build -t local/app .
muffle --log build.log -- cmake --build build
muffle --verbose -- cargo test
muffle -- sh -c 'generate-files | sort > sorted.txt'
```

For GitHub Actions, install/build Muffle in a prior step and invoke it as a
normal command:

```yaml
- name: Run tests with concise output
  run: ./build/muffle -- npm test
```

Muffle passes arguments as given. Shell operators such as pipes, redirects,
variable expansion, and `&&` only work when you explicitly invoke a shell,
for example `muffle -- sh -c 'build-command && test-command'`.

See [15+ verified usage patterns](docs/examples.md) for success, failure,
logging, shell behavior, and development workflows.

## Usage reference

```text
muffle [--tail N] [--verbose] [--log PATH] -- COMMAND [ARG ...]
muffle --help | --version
```

| Option | Meaning |
| --- | --- |
| `-h`, `--help` | Print usage and exit successfully. |
| `--version` | Print the compiled version and exit successfully. |
| `--tail N` | Show the final positive `N` lines on failure; default is 40. |
| `--verbose` | Stream captured output while the command runs. |
| `--log PATH` | Keep a log; destination must not exist. |
| `--` | End Muffle options; following tokens are the command. |

Use `--` even if the command looks simple. It prevents the parser from
mistaking child arguments for Muffle options, and preserves shell-looking
characters literally. Full syntax, parsing errors, and examples are in the
[CLI reference](docs/cli-reference.md).

## Exit codes and errors

A normal child exit code is returned unchanged. Signal termination returns
`128 + signal number`. Missing commands return 127; other `execvp` failures
return 126. Muffle internal failures return 125, and invalid CLI arguments
return 2. See the [exit-code reference](docs/exit-codes.md).

## Architecture

Muffle parses options, creates a private log and capture pipe, forks, and
replaces the child process with the requested executable. The parent copies
combined output to the log, optionally streams it, and prints a bounded tail
on failure. See [architecture](docs/architecture.md),
[process execution](docs/process-execution.md), and
[signal handling](docs/signal-handling.md).

## Contributing and community

Bug reports, focused feature proposals, and documentation fixes are welcome.
Start with [CONTRIBUTING.md](CONTRIBUTING.md), then use the repository's
[issue templates](https://github.com/Thavarshan/muffle/issues/new/choose).
Review expectations and the maintainer workflow are documented there. The
maintainer works directly on `main` for routine work; external contributors
can submit pull requests.

- [Code of conduct](CODE_OF_CONDUCT.md)
- [Security policy](SECURITY.md)
- [Support](SUPPORT.md)
- [Governance](GOVERNANCE.md)
- [Changelog](CHANGELOG.md)

## License and maintenance

Muffle is licensed under the [MIT License](LICENSE). Security support and
vulnerability reporting guidance are in [SECURITY.md](SECURITY.md). Muffle
is an early versioned project (`0.1.2`). Check the [release page][releases]
for current assets and the [installation guide](docs/installation.md) for
verified package channels.

## Documentation

- [Documentation index](docs/index.md)
- [Getting started](docs/getting-started.md)
- [Installation](docs/installation.md)
- [CLI reference](docs/cli-reference.md)
- [Examples](docs/examples.md)
- [Exit codes](docs/exit-codes.md)
- [Logging](docs/logging.md)
- [Security notes](docs/security.md)
- [Architecture](docs/architecture.md)
- [Process execution](docs/process-execution.md)
- [Signal handling](docs/signal-handling.md)
- [Supported platforms](docs/platforms.md)
- [Configuration and environment](docs/configuration.md)
- [Development](docs/development.md)
- [Testing](docs/testing.md)
- [Quality standards](docs/quality-standards.md)
- [Troubleshooting](docs/troubleshooting.md)
- [FAQ](docs/faq.md)
- [Limitations](docs/limitations.md)
- [Roadmap](docs/roadmap.md)
- [Release process](docs/releasing.md)
- [Maintainer repository settings](docs/maintainers/repository-settings.md)
- [Maintainer maintenance notes](docs/maintainers/maintenance.md)

## Repository links

- [GitHub Discussions](https://github.com/Thavarshan/muffle/discussions)
- [Issues](https://github.com/Thavarshan/muffle/issues)
- [CI workflow](.github/workflows/ci.yml)

[ci]: https://github.com/Thavarshan/muffle/actions/workflows/ci.yml/badge.svg
[ci-workflow]: https://github.com/Thavarshan/muffle/actions/workflows/ci.yml
[releases]: https://github.com/Thavarshan/muffle/releases
[release-badge]: https://img.shields.io/github/v/release/Thavarshan/muffle
[license-badge]: https://img.shields.io/github/license/Thavarshan/muffle
[cpp]: https://img.shields.io/badge/C%2B%2B-17-blue
[platform]: https://img.shields.io/badge/platform-macOS%20%26%20Linux-blue
