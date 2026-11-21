"""Black-box process and output tests for the Muffle CLI."""

import os
from pathlib import Path
import signal
import subprocess
import sys
import tempfile
import time
import unittest


class CliIntegrationTests(unittest.TestCase):
    """Exercise real processes with an isolated temporary directory."""

    binary = sys.argv[1]

    def setUp(self):
        """Create a private directory for captured output and markers."""
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.directory = Path(self.temp.name)
        self.environment = dict(os.environ, TMPDIR=self.temp.name)

    def invoke(self, *arguments):
        """Run Muffle, capturing its actual status and streams."""
        return subprocess.run(
            [self.binary, *arguments],
            check=False,
            capture_output=True,
            text=True,
            env=self.environment,
            timeout=15,
        )

    def child(self, code, *flags):
        """Run a Python subprocess through Muffle without a shell."""
        return self.invoke(*flags, "--", sys.executable, "-c", code)

    def test_help_and_version(self):
        """Print meaningful usage and version information."""
        self.assertIn("Usage: muffle", self.invoke("--help").stdout)
        self.assertRegex(self.invoke("--version").stdout, r"muffle \d")

    def test_invalid_arguments(self):
        """Reject malformed flags before starting a command."""
        result = self.invoke("--tail", "0", "--", "echo")
        self.assertEqual(result.returncode, 2)
        self.assertIn("positive integer", result.stderr)

    def test_success_suppresses_output(self):
        """Hide successful stdout and stderr."""
        result = self.child("print('secret-output')")
        self.assertEqual(result.returncode, 0)
        self.assertIn("muffle: passed", result.stdout)
        self.assertNotIn("secret-output", result.stdout)
        self.assertEqual(result.stderr, "")
        self.assertEqual(list(self.directory.glob("muffle-*")), [])

    def test_failure_returns_exact_status_and_tail(self):
        """Preserve nonzero child codes and the final output lines."""
        code = "import sys;print('first');print('last');sys.exit(42)"
        result = self.child(code, "--tail", "1")
        self.assertEqual(result.returncode, 42)
        self.assertIn("exit 42", result.stderr)
        self.assertIn("last\n", result.stderr)
        self.assertNotIn("first\n", result.stderr)
        self.assertIn("Log: ", result.stderr)

    def test_combined_stderr_and_stdout(self):
        """Capture both standard streams in one log."""
        code = (
            "import sys;print('out',flush=True);"
            "print('err',file=sys.stderr,flush=True);sys.exit(3)"
        )
        result = self.child(code)
        self.assertEqual(result.returncode, 3)
        self.assertIn("out\n", result.stderr)
        self.assertIn("err\n", result.stderr)

    def test_verbose_streams_successful_output(self):
        """Show normally hidden bytes when verbose mode is enabled."""
        result = self.child("print('visible')", "--verbose")
        self.assertEqual(result.returncode, 0)
        self.assertIn("visible\n", result.stdout)
        self.assertIn("muffle: passed", result.stdout)

    def test_log_is_private_and_persistent(self):
        """Keep the chosen log and restrict new-file permissions."""
        log = self.directory / "capture.log"
        result = self.child("print('stored')", "--log", str(log))
        self.assertEqual(result.returncode, 0)
        self.assertEqual(log.read_text(), "stored\n")
        self.assertEqual(log.stat().st_mode & 0o777, 0o600)

    def test_existing_log_is_never_overwritten(self):
        """Refuse to replace existing content or follow symlinks."""
        log = self.directory / "capture.log"
        log.write_text("precious")
        result = self.child("print('do not run')", "--log", str(log))
        self.assertEqual(result.returncode, 125)
        self.assertEqual(log.read_text(), "precious")

    def test_argument_is_not_shell_interpreted(self):
        """Preserve literal shell metacharacters in child arguments."""
        code = "import sys;print(sys.argv[1])"
        result = self.invoke(
            "--verbose", "--", sys.executable, "-c", code, ";$(id)"
        )
        self.assertEqual(result.returncode, 0)
        self.assertIn(";$(id)", result.stdout)

    def test_missing_executable_returns_127(self):
        """Report a missing executable without throwing exceptions."""
        result = self.invoke("--", "muffle-unknown-executable-xyz")
        self.assertEqual(result.returncode, 127)
        self.assertIn("exec failed", result.stderr)

    def test_no_trailing_newline(self):
        """Include the last partial line when displaying failures."""
        code = "import sys;sys.stdout.write('partial');sys.exit(6)"
        result = self.child(code)
        self.assertEqual(result.returncode, 6)
        self.assertIn("partial\nLog:", result.stderr)

    def test_many_lines_tail_is_bounded(self):
        """Do not display thousands of irrelevant lines on failure."""
        code = (
            "import sys;"
            "[print('line-'+str(n)) for n in range(5000)];"
            "sys.exit(9)"
        )
        result = self.child(code, "--tail", "3")
        self.assertEqual(result.returncode, 9)
        self.assertIn("line-4997\n", result.stderr)
        self.assertIn("line-4999\n", result.stderr)
        self.assertNotIn("line-4996\n", result.stderr)
        self.assertLess(len(result.stderr), 500)

    def test_parent_signal_reaches_child(self):
        """Forward SIGTERM and return the conventional signal status."""
        marker = self.directory / "ready"
        code = (
            "from pathlib import Path;import time;"
            f"Path({str(marker)!r}).write_text('ready');"
            "time.sleep(30)"
        )
        process = subprocess.Popen(
            [self.binary, "--", sys.executable, "-c", code],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            env=self.environment,
            text=True,
        )
        try:
            deadline = time.monotonic() + 5
            while not marker.exists() and time.monotonic() < deadline:
                time.sleep(0.01)
            self.assertTrue(marker.exists(), "child did not start")
            process.send_signal(signal.SIGTERM)
            stdout, stderr = process.communicate(timeout=5)
            self.assertEqual(process.returncode, 143)
            self.assertIn("exit 143", stderr)
            self.assertEqual(stdout, "")
        finally:
            if process.poll() is None:
                process.kill()
                process.communicate(timeout=5)


if __name__ == "__main__":
    executable = sys.argv[1]
    sys.argv = [sys.argv[0]]
    CliIntegrationTests.binary = executable
    unittest.main()
