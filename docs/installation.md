# Installation

Muffle runs on macOS and Linux systems with POSIX process APIs. Current
published channels are listed below; a package format is available only after
its release artifact has been uploaded and verified. See the [GitHub Releases
page](https://github.com/Thavarshan/muffle/releases) for actual versions and
assets.

## Homebrew

The maintained tap builds Muffle from its immutable source tag. Install,
verify, update, and remove it with:

```sh
brew install Thavarshan/muffle/muffle
muffle --version
brew update
brew upgrade muffle
brew uninstall muffle
```

On Homebrew 7, review and approve the formula trust prompt for this third-party
tap if shown. Linuxbrew can use the same source formula where Homebrew and its
CMake dependency are supported.

## GitHub Release archives

Choose the archive matching your OS and CPU architecture from the release
assets. Extract it and put `bin/muffle` somewhere on your PATH. For example:

```sh
mkdir -p "$HOME/.local"
tar -xzf muffle-vVERSION-PLATFORM.tar.gz -C "$HOME/.local"
"$HOME/.local/bin/muffle" --version
```

Replace `VERSION` and `PLATFORM` with the exact asset name shown on the
release page. The archive contains the executable, project README, license,
and manual page. Verify an asset against `SHA256SUMS` with `sha256sum -c
SHA256SUMS` (or `shasum -a 256 -c SHA256SUMS` on macOS). Release workflow
provenance attestations are provided when shown on the release page.

To update, download and verify a newer archive, then replace the prior
executable. Remove the installed executable and documentation files to
uninstall. The archive does not install a package-manager database entry.

## Debian and Ubuntu

When `.deb` assets are present in a verified release, download the one matching
your architecture and install it with:

```sh
sudo apt install ./muffle_VERSION_ARCH.deb
muffle --version
sudo apt remove muffle
```

Replace the filename placeholders with the exact asset name. These files are
standalone release packages; they are not an APT repository, so `apt install
muffle` without a local package is not provided by this project.

## Fedora and RHEL family

When `.rpm` assets are present in a verified release, download the matching
package and use:

```sh
sudo dnf install ./muffle-VERSION-RELEASE.ARCH.rpm
muffle --version
sudo dnf remove muffle
```

These downloads are not an enabled DNF repository. Fedora/RHEL runtime
compatibility depends on the runtime libraries of the build environment; see
[platform notes](platforms.md).

## Build from source

### Prerequisites

- CMake 3.20 or newer
- A C++17 compiler: GCC, Clang, or Apple Clang
- Python 3 for CTest integration tests
- `clang-format`, `clang-tidy`, Doxygen, and PyYAML for the complete quality
  gate

### macOS

Install Xcode Command Line Tools and CMake, then build and test:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/muffle --version
```

### Ubuntu and Debian

```sh
sudo apt-get update
sudo apt-get install -y build-essential cmake python3
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/muffle --version
```

On other Linux distributions, install CMake 3.20+, a C++17-capable GCC or
Clang toolchain, and Python 3, then use the same CMake commands.

### Debug and Release builds

Use `-DCMAKE_BUILD_TYPE=Debug` during development or `Release` for an
optimized build. Build into separate directories to keep both configurations.

### Install with CMake

CMake installs the executable under `bin`, the manual page under
`share/man/man1`, and project documentation under `share/doc/muffle`. A
user-local install can use:

```sh
cmake --install build --prefix "$HOME/.local"
"$HOME/.local/bin/muffle" --version
```

Add `$HOME/.local/bin` to PATH to run `muffle` without its full path. CMake
does not generate an uninstall target; remove the installed files listed in
`build/install_manifest.txt` manually.

## Nix

A source flake is maintained in `flake.nix`. Use it after installing Nix with
flakes enabled:

```sh
nix build github:Thavarshan/muffle
./result/bin/muffle --version
```

This uses the current default branch. For a pinned release, append the
verified release tag to the GitHub reference. The flake has not yet been
published in the official nixpkgs collection.

## Updating a source checkout

```sh
git pull --ff-only
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
```

## Not currently published

The project does not currently publish an APT or DNF repository, AUR package,
conda-forge package, or Snap. Do not use `apt install muffle`, `dnf install
muffle`, `yay -S muffle`, or `conda install muffle` as project installation
commands. See [release maintenance](releasing.md) for submission status and
requirements.

## Troubleshooting PATH

If `muffle --version` reports that the command is not found, check
`command -v muffle`, run the installed binary by absolute path, and add the
installation prefix's `bin` directory to PATH. Open a new shell or reload its
startup file after changing PATH.
