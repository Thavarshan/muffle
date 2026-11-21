# Contributing

Use C++17, 80-column lines and explicit, documented public contracts.
Avoid adding dependencies unless they offer a measurable advantage.

All public headers must use Doxygen documentation for public classes,
functions, arguments and return values. Documentation warnings fail CI.
Do not commit commented-out code. `TODO`, `FIXME`, or `HACK` comments
must reference a numbered GitHub issue, for example `// TODO #12: ...`.

Before committing:

```sh
bash scripts/check.sh
```

Install the repository-owned hook once per local clone:

```sh
bash scripts/setup-hooks.sh
```

Changes to the command runner should include both deterministic unit
tests and end-to-end process tests; these must verify actual exit codes,
stdout/stderr handling, and signal forwarding. Do not claim coverage for
a feature that has not yet been implemented.
