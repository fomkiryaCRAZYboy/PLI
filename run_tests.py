#!/usr/bin/env python3
"""Run PLI interpreter on .pli test files."""

import sys
import os
import subprocess
import glob

PLI_BIN = os.path.join(os.path.dirname(__file__), "build", "bin", "pli")

def run_file(path):
    print(f"--- {os.path.basename(path)} ---")
    result = subprocess.run(
        [PLI_BIN, path],
        capture_output=True,
        text=True,
        timeout=10,
    )
    if result.stdout:
        print(result.stdout, end="")
    if result.stderr:
        print(result.stderr, end="")
    if result.returncode != 0:
        print(f"[EXIT CODE: {result.returncode}]")
    print()

def main():
    if not os.path.isfile(PLI_BIN):
        print(f"Error: pli binary not found at {PLI_BIN}", file=sys.stderr)
        print("Run: cd build && cmake .. && cmake --build . --target pli", file=sys.stderr)
        sys.exit(1)

    files = []
    for arg in sys.argv[1:]:
        if os.path.isdir(arg):
            files.extend(sorted(glob.glob(os.path.join(arg, "*.pli"))))
        elif os.path.isfile(arg):
            files.append(arg)
        else:
            found = sorted(glob.glob(arg))
            if found:
                files.extend(found)
            else:
                print(f"Warning: {arg} not found, skipping", file=sys.stderr)

    if not files:
        print("Usage: python3 run_tests.py <file.pli | directory | glob>", file=sys.stderr)
        print("Examples:", file=sys.stderr)
        print("  python3 run_tests.py test_parser/", file=sys.stderr)
        print("  python3 run_tests.py test_parser/test01_simple_literals.pli", file=sys.stderr)
        print("  python3 run_tests.py test_parser/*.pli", file=sys.stderr)
        sys.exit(1)

    passed = 0
    failed = 0

    for f in files:
        try:
            run_file(f)
            passed += 1
        except subprocess.TimeoutExpired:
            print(f"--- {os.path.basename(f)} ---")
            print("[TIMEOUT]")
            print()
            failed += 1
        except Exception as e:
            print(f"--- {os.path.basename(f)} ---")
            print(f"[ERROR: {e}]")
            print()
            failed += 1

    print(f"=== {passed + failed} files: {passed} ok, {failed} failed ===")

if __name__ == "__main__":
    main()
