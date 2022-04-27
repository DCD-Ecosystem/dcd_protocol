#!/usr/bin/env python3

# This script tests that dcdcli launches dcdksd automatically when dcdksd is not
# running yet.

import subprocess


def run_dcdcli_wallet_command(command: str, no_auto_dcdksd: bool):
    """Run the given dcdcli command and return subprocess.CompletedProcess."""
    args = ['./programs/dcdcli/dcdcli']

    if no_auto_dcdksd:
        args.append('--no-auto-dcdksd')

    args += 'wallet', command

    return subprocess.run(args,
                          check=False,
                          stdout=subprocess.DEVNULL,
                          stderr=subprocess.PIPE)


def stop_dcdksd():
    """Stop the default dcdksd instance."""
    run_dcdcli_wallet_command('stop', no_auto_dcdksd=True)


def check_dcdcli_stderr(stderr: bytes, expected_match: bytes):
    if expected_match not in stderr:
        raise RuntimeError("'{}' not found in {}'".format(
            expected_match.decode(), stderr.decode()))


def dcdksd_auto_launch_test():
    """Test that keos auto-launching works but can be optionally inhibited."""
    stop_dcdksd()

    # Make sure that when '--no-auto-dcdksd' is given, dcdksd is not started by
    # dcdcli.
    completed_process = run_dcdcli_wallet_command('list', no_auto_dcdksd=True)
    assert completed_process.returncode != 0
    check_dcdcli_stderr(completed_process.stderr, b'Failed to connect to dcdksd')

    # Verify that dcdksd auto-launching works.
    completed_process = run_dcdcli_wallet_command('list', no_auto_dcdksd=False)
    if completed_process.returncode != 0:
        raise RuntimeError("Expected that dcdksd would be started, "
                           "but got an error instead: {}".format(
                               completed_process.stderr.decode()))
    check_dcdcli_stderr(completed_process.stderr, b'launched')


try:
    dcdksd_auto_launch_test()
finally:
    stop_dcdksd()
