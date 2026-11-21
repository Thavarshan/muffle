# CLI reference

## Syntax

```text
muffle [--tail N] [--verbose] [--log PATH] -- COMMAND [ARG ...]
muffle --help | --version
```

Options are parsed before the required `--` separator. After `--`, Muffle
passes every token unchanged to the child process. The command must not be
empty. Muffle does not invoke a shell implicitly.

| Option | Default | Behavior |
| --- | --- | --- |
| `-h`, `--help` | — | Print usage to stdout and exit 0. |
| `--version` | — | Print the compiled version to stdout and exit 0. |
| `--tail N` | 40 lines | Show the final positive N lines on failure. |
| `--verbose` | off | Stream captured bytes to Muffle's stdout. |
| `--log PATH` | temporary log | Persistent mode 0600 file; no overwrite. |
| `--` | required | Stop parsing Muffle options and begin the child command. |

`--tail` accepts a positive base-10 integer representable by `size_t`.
`--log` requires a non-empty path. Options can be specified in any order
before `--`; repeated values are accepted and the last `--tail` or `--log`
value takes effect. `--verbose` is a boolean flag.

## Examples

```sh
muffle -- cmake --build build
muffle --tail 10 -- make test
muffle --verbose -- cargo test
muffle --log build.log -- npm run build
muffle --verbose -- sh -c 'printf "a\\nb\\n" | sort'
```

## Errors

Invalid or incomplete CLI syntax prints an error and `Use --help for usage.`
to stderr and exits 2. This includes an unknown pre-separator option, a
missing `--`, invalid tail value, missing option value, or an empty command
after `--`. A token resembling an option after `--` is a child argument.

See [exit codes](exit-codes.md) and [logging](logging.md) for execution-time
failures and file behavior.
