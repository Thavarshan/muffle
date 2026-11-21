# Security policy

## Supported versions

Security fixes are considered for the latest source on the `main` branch.
No prior release series currently receives a separate support commitment;
there are no published releases at this time.

## Reporting a vulnerability

Use GitHub's **Report a vulnerability** feature in the repository's
Security tab to send a private report when it is available. Do not initially
open a public issue or disclose exploit details publicly. Include affected
version or commit, platform, reproduction steps, impact, and any suggested
mitigation. Do not include real secrets or personal data in the report.

The maintainer will review the report and coordinate a reasonable disclosure
with the reporter. The project does not promise a response deadline. Avoid
public disclosure until maintainers and the reporter have had an opportunity
to assess and address the issue.

## Security-sensitive areas

Areas that deserve particular care include argument handling and `execvp`,
file creation and log permissions, temporary path handling, descriptor
management, process groups, signal forwarding, and error cleanup.

Muffle runs children with the invoking user's privileges. It does not
sandbox, privilege-separate, redact, or encrypt child output. Logs can
contain sensitive data. Signal forwarding and cleanup are best-effort; no
complete process-tree termination guarantee is made.
