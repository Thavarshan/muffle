# Signal handling

The runner installs handlers for SIGINT and SIGTERM before launching the
child. While a child is active, it records the child process-group identifier
and forwards those signals to the group using `kill(-pgid, signal)`, falling
back to the child PID if the group signal fails. The child attempts to create
its own process group.

If the parent receives SIGINT or SIGTERM while capturing output or waiting
for the child, it stops capture when needed, forwards the signal, and waits up
to 500 milliseconds for the direct child. It then sends SIGKILL to the child
group and PID if needed. After a direct
child exits during this cleanup, it also attempts to kill remaining members
of that process group.

When the child exits due to a signal, Muffle returns `128 + signal number`.
For example, SIGTERM normally maps to 143. The integration suite exercises
SIGTERM forwarding to a directly launched child.

This is limited best-effort forwarding, not a guarantee for every race,
signal, shell arrangement, or descendant process. Process-group setup can
fail, descendants can create separate groups, and signals such as SIGKILL
cannot be caught or forwarded by a handler. If capture fails, the runner
attempts SIGKILL cleanup for the child group and PID. See
[limitations](limitations.md).
