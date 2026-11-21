# Logging and output

## Capture model

Muffle creates one pipe, redirects both child stdout and stderr to its write
end, then copies bytes read from the pipe into a file. This keeps memory use
bounded by a fixed-size read buffer. Because both child streams share one
pipe, the result has their observed pipe-write order, not a reconstruction of
independent stdout/stderr ordering. Buffering inside the child can also
change when bytes reach the pipe.

By default, Muffle prints no child output while it runs. On success it prints
`muffle: passed (SECONDSs)` to stdout and deletes its temporary log. On
failure it prints a status summary and up to the final 40 lines to stderr,
then reports the retained log path. `--tail N` changes the line limit.

`--verbose` writes captured bytes to Muffle's stdout as they are read, even
when those bytes originated on child stderr. It does not disable capture or
change the log policy.

## Temporary and persistent logs

Without `--log`, Muffle uses `TMPDIR` when set, or `/tmp` otherwise, and
creates a uniquely named `muffle-XXXXXX` file with `mkstemp`. The file is
created privately (normally mode 0600). It is deleted after child success;
on failure it remains so the full output can be inspected. The exact path is
printed in the failure summary.

With `--log PATH`, Muffle opens PATH with exclusive create semantics and
requests mode 0600 (the process umask may make it more restrictive). It
refuses an existing file or symlink and does not overwrite it. The persistent
log remains after either success or failure. Changing its permissions is the
caller's responsibility.

Temporary and persistent logs can grow as large as child output. Ensure the
temporary filesystem or chosen destination has space. A failed write or
capture operation is an internal failure (125); the child is terminated
where possible and the log path is reported when available.

## Privacy and cleanup

Logs may contain credentials, source snippets, customer data, or other
sensitive output emitted by the child. Choose a private destination, inspect
before sharing, and remove logs when no longer needed. Successful temporary
logs are automatically removed; failure logs and all `--log` files require
manual cleanup. Muffle does not redact output or encrypt logs.
