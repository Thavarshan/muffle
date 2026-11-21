# Security notes

Muffle runs a command selected by its caller. It passes arguments directly to
`execvp`, rather than evaluating a command string in a shell. This avoids
Muffle itself interpreting shell metacharacters; it does not make an
untrusted executable or its arguments safe. A child can access the same
resources and credentials available to the invoking user.

Muffle captures child stdout and stderr into a local file. New log files use
mode 0600, and persistent log creation refuses to overwrite an existing
path. Logs are not encrypted or redacted and can contain secrets emitted by
the child. Temporary failure logs are retained. Treat them as sensitive.

The code uses `fork`, `execvp`, pipes, process groups, and signal handlers.
Signal forwarding and cleanup are best-effort and do not sandbox the child,
limit its privileges, or guarantee termination of every descendant. Muffle
makes no broader security guarantee.

Report suspected vulnerabilities through GitHub's private vulnerability
reporting feature when available. Do not initially disclose exploit details
in a public issue. See [SECURITY.md](../SECURITY.md) for the repository's
reporting policy and support scope.
