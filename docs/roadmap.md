# Roadmap

## Shipped in the current source

Muffle supports direct POSIX child execution, combined output capture,
quiet-on-success behavior, failure tails, optional verbose streaming,
persistent logs, child exit-status propagation, and best-effort SIGINT and
SIGTERM forwarding. See [CLI reference](cli-reference.md) and
[limitations](limitations.md).

## Potential improvements

These are ideas, not committed delivery promises or implemented features:

- Improve test coverage for unusual file and process failures.
- Evaluate whether configurable output stream handling has a useful design.
- Evaluate bounded log storage or rotation for very large output.
- Consider broader POSIX platform testing beyond the current CI matrix.

No release schedule or compatibility guarantee beyond the documented
current behavior is established yet. Discuss significant proposals in an
issue before implementation.
