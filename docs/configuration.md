# Configuration and environment

Muffle has no configuration file. Its supported behavior is selected through
CLI flags. The only environment variable read directly by the runner is
`TMPDIR`, which selects the directory for temporary logs. If unset, Muffle
uses `/tmp`.

The child inherits the invoking process environment through `execvp`,
including `PATH`; PATH search is provided by `execvp` when the command name
has no slash. Muffle does not read a project config, modify the environment,
or load plugins.

See [CLI reference](cli-reference.md), [logging](logging.md), and
[process execution](process-execution.md).
