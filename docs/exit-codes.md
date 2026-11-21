# Exit codes

Muffle's process result is suitable for scripts: it returns a normal child
exit status unchanged and uses reserved statuses for failures it detects.

| Situation | Muffle status | Notes |
| --- | ---: | --- |
| Help or version displayed | 0 | No child is started. |
| Child exits 0 | 0 | Muffle prints a success summary. |
| Child exits nonzero | Same status |
| Muffle prints the configured tail and retains its log. |
| Child is killed by signal N | 128 + N |
| POSIX conventional mapping; e.g. SIGTERM (15) becomes 143. |
| `execvp` reports `ENOENT` | 127 |
| Includes a missing executable and missing executable interpreter. |
| Other `execvp` error | 126 |
| For example, permission denied or another execution error. |
| Muffle internal failure | 125 |
| Log creation, pipe/fork, capture, or wait failure. |
| Invalid command-line arguments | 2 | Rejected before launching the child. |

An inaccessible or invalid log path fails before the child starts and returns
125. On a capture or wait failure, Muffle attempts to terminate the child and
reports the log path. If an internal failure occurs after log creation, the
log is retained. See [logging](logging.md) for details.
