# Installation

Muffle currently provides source and CMake install instructions. The project
has no published packages or prebuilt binaries. Its process runner uses POSIX
APIs and is intended for macOS and Linux.

## Prerequisites

- CMake 3.20 or newer
- A C++17 compiler: GCC, Clang, or Apple Clang
- Python 3 to configure and run tests
- `clang-format`, `clang-tidy`, and Doxygen only for the full quality script

## macOS

Install Xcode Command Line Tools and CMake (for example, using the official
CMake installer or a package manager), then build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
./build/muffle --version
```

## Ubuntu and Debian

Install a compiler, CMake, and Python 3 using the system package manager:

```sh
sudo apt-get update
sudo apt-get install -y build-essential cmake python3
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
./build/muffle --version
```

## Other Linux distributions

Install CMake 3.20+, a C++17-capable GCC or Clang toolchain, and Python 3
using your distribution's package manager. Then use the same configure and
build commands. No distribution-specific package is maintained by this
repository.

## Debug and Release builds

Use `-DCMAKE_BUILD_TYPE=Debug` for development or `Release` for an optimized
build. Build in a separate directory if you want to keep both:

```sh
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug --parallel 2
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --parallel 2
```

## Install with CMake

CMake installs the `muffle` executable under `bin` in the selected prefix.
For a user-local installation, choose a writable prefix:

```sh
cmake --install build --prefix "$HOME/.local"
"$HOME/.local/bin/muffle" --version
```

To use `muffle` without the full path, add `$HOME/.local/bin` to your shell's
PATH. For a system prefix such as `/usr/local`, use the platform's normal
administrative install procedure. CMake does not currently generate an
uninstall target; remove the installed `bin/muffle` executable manually.

## Update from Git

Pull updates in the checkout, then reconfigure and rebuild:

```sh
git pull --ff-only
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
```

## Troubleshooting PATH

If `muffle --version` reports that the command is not found, check
`command -v muffle`, confirm the install prefix's `bin` directory is on PATH,
and open a new shell or reload its startup file. Running the executable by
absolute path distinguishes PATH configuration from a build or install
problem.
