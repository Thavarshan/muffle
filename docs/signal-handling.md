# Signal handling

The runner installs handlers for SIGINT and SIGTERM before launching the
child. While a child is active, it records the child process-group identifier
and forwards those signals to the group using `kill(-pgid, signal)`, falling
back to the child PID if the group signal fails. The child attempts to create
its own process group.

When the child exits due to a signal, Muffle returns `128 + signal number`.
For example, SIGTERM normally maps to 143. The integration suite exercises
SIGTERM forwarding to a directly launched child.

This is limited best-effort forwarding, not a guarantee for every race,
signal, shell arrangement, or descendant process. Process-group setup can
fail, descendants can create separate groups, and signals such as SIGKILL
cannot be caught or forwarded by a handler. If capture fails, the runner
attempts SIGKILL cleanup for the child group and PID. See
[limitations](limitations.md).
