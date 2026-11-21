# Getting started

Muffle runs a child program and captures its stdout and stderr together. It
hides output when the child succeeds. On failure it reports the child status,
prints the final output lines, and retains the log. It does not parse a shell
command string or provide an interactive terminal.

## Build and run

Install CMake 3.20 or newer, a C++17 compiler, and Python 3. Clone the
repository and build it:

```sh
git clone https://github.com/Thavarshan/muffle.git
cd muffle
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
```

Run a command after `--`:

```sh
./build/muffle -- printf 'hello\n'
```

The child output is captured and discarded after success. Muffle prints a
short `muffle: passed` message. To see output while running, use
`--verbose`. To retain a log on success, pass a path that does not yet exist
to `--log`:

```sh
./build/muffle --verbose -- npm test
./build/muffle --log test.log -- ctest --test-dir build
```

To inspect failure behavior, run a deliberately failing child:

```sh
./build/muffle --tail 10 -- sh -c 'echo failed; exit 42'
```

Muffle returns 42 and prints the captured tail to stderr. This example
explicitly starts `sh`; Muffle itself does not interpret `;`, pipes, quotes,
redirections, or variable expansions.

See [installation](installation.md) for install commands and
[CLI reference](cli-reference.md) for every supported option.
