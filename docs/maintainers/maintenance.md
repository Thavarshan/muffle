# Maintainer maintenance notes

Muffle currently uses a single-maintainer model. The maintainer handles
routine work directly on `main`, reviews external pull requests, makes
scope and release decisions, and updates repository metadata. No committee
or formal organization is defined.

A lightweight maintenance cycle:

1. Review new issues and label bug, enhancement, documentation, question,
   testing, security, or platform reports as appropriate.
2. Reproduce bugs and capture relevant environment and exit status details.
3. Keep docs aligned with source and tests when user-visible behavior changes.
4. Run the local quality gate and review the GitHub Actions result after
   publishing to `main`.
5. Close or update stale reports with a clear reason and current status.
6. Review dependency/security alerts when GitHub reports them.

There are no service-level response deadlines. Use GitHub private
vulnerability reporting for suspected security issues.
