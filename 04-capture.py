#!/usr/bin/env python3
"""Part 3: the same capture bug as 04-capture-broken.c, in Python.

usage: python3 04-capture.py N [broken|fixed]      (default: fixed)

Python's own documentation for Popen.wait() warns about exactly this
(https://docs.python.org/3/library/subprocess.html#subprocess.Popen.wait):

    This will deadlock when using stdout=PIPE or stderr=PIPE and the child
    process generates enough output to a pipe such that it blocks waiting
    for the OS pipe buffer to accept more data. Use Popen.communicate()
    when using pipes to avoid that.
"""
import subprocess
import sys


def main():
    if len(sys.argv) not in (2, 3):
        sys.exit("usage: python3 04-capture.py N [broken|fixed]")
    n = sys.argv[1]
    mode = sys.argv[2] if len(sys.argv) == 3 else "fixed"

    p = subprocess.Popen(["./04-worker", n], stdout=subprocess.PIPE)

    if mode == "broken":
        print("broken: p.wait() before p.stdout.read() ... "
              "(hangs if N > pipe capacity; Ctrl-C, or run under timeout)",
              file=sys.stderr)
        p.wait()                    # blocks until the child exits ...
        data = p.stdout.read()      # ... but the child is blocked until we read
    elif mode == "fixed":
        data, _ = p.communicate()   # reads to EOF, then waits
    else:
        sys.exit(f"unknown mode {mode!r}: use broken or fixed")

    print(f"captured {len(data)} bytes, exit status {p.returncode}")


if __name__ == "__main__":
    main()
