# Examples

All examples below require the named command to be installed separately.
Muffle executes the first token directly and passes remaining arguments
literally.

## Quiet success and useful failures

```sh
muffle -- printf 'generated files\n'
muffle --tail 20 -- cmake --build build
muffle --tail 80 -- ctest --test-dir build --output-on-failure
muffle -- make -j2
muffle -- cargo test
muffle -- npm test
muffle -- npm run build
muffle -- composer install
muffle -- ./vendor/bin/phpunit
muffle -- docker build -t local/app .
```

For a command known to fail, use `sh -c` explicitly:

```sh
muffle --tail 5 -- sh -c 'echo setup; echo failure >&2; exit 42'
```

The child receives 42; the tail is printed to Muffle's stderr. `--verbose`
streams output while it runs, including output from a command that later
fails:

```sh
muffle --verbose -- cargo test
muffle --verbose -- npm test
```

## Persistent logs

The destination must not exist. These commands create `build.log` and
`test.log` with mode 0600, subject to the process umask (0600 remains
restricted):

```sh
muffle --log build.log -- cmake --build build
muffle --log test.log -- ctest --test-dir build
muffle --log deploy.log -- ./scripts/deploy-staging.sh
```

Inspect a retained log with ordinary tools:

```sh
tail -n 100 test.log
less build.log
```

## Shell features and argument safety

Muffle does not parse shell syntax. This passes literal `;` and `$()` bytes
to `printf`; it does not run `id`:

```sh
muffle -- printf '%s\n' '; $(id)'
```

To use a pipe or redirection, invoke a shell and quote its program text:

```sh
muffle --verbose -- sh -c 'printf "a\\nb\\n" | sort'
muffle -- sh -c 'make > build-output.txt 2>&1'
muffle -- sh -c 'cmake --build build && ctest --test-dir build'
```

These shell commands are examples of explicit shell use, not a Muffle shell
feature. Their combined output is captured as one stream.

## CI example

```yaml
- name: Build
  run: cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
- name: Compile
  run: cmake --build build --parallel 2
- name: Test with concise output
  run: ./build/muffle -- ctest --test-dir build --output-on-failure
```

This assumes a preceding step installed the repository dependencies and
built Muffle. The command's exit status remains visible to the workflow.

The examples involving `printf`, `sh`, and the Muffle-built executable can
be checked locally on macOS and Linux. Examples involving npm, Composer,
PHPUnit, CMake, Make, Cargo, Docker, and a project deployment script depend
on the respective tool and project being available.
