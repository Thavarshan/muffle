# Process execution

Muffle receives an argument vector, not a shell command string. After `--`,
it stores the command and arguments as strings and passes pointers to
`execvp`. The child process is created with `fork`; it redirects stdout and
stderr to a pipe with `dup2`, closes unneeded descriptors, then calls
`execvp`.

`execvp` searches PATH when the executable name contains no slash and uses
the supplied argument vector. Muffle does not expand quotes, glob patterns,
variables, pipes, redirects, substitutions, or control operators. To request
those shell behaviors, name a shell explicitly, for example:

```sh
muffle -- sh -c 'printf "hello\\n" | tr a-z A-Z'
```

Passing arguments directly avoids Muffle's own shell-string parsing and
preserves each argument boundary. It does not validate the command, sanitize
arguments for the child, or restrict child privileges. Never pass
untrusted input into a shell program without applying the shell's own safe
argument handling.

POSIX `execvp` may fall back to `/bin/sh` when the selected executable file
is rejected with `ENOEXEC` (for example, an executable text file without a
valid format or interpreter line). This is `execvp` behavior for the
executable; Muffle does not otherwise parse the argument vector as shell
syntax.

The parent uses a pipe to receive combined child output and writes it to a
log using file descriptors. `waitpid` collects the child status. See
[logging](logging.md) for stream ordering limitations and
[exit codes](exit-codes.md) for status mapping. Interactive TTY-dependent
programs are unsupported because the child is connected to a pipe rather
than the caller's terminal.
