#!/usr/bin/env bash

set -euo pipefail

failures=0

run_expect_fail() {
    name="$1"
    shift

    if "$@" >/tmp/parallel_search_invalid_stdout.txt 2>/tmp/parallel_search_invalid_stderr.txt; then
        echo "FAIL: $name succeeded but failure was expected"
        failures=$((failures + 1))
    else
        echo "PASS: $name failed as expected"
    fi
}

make >/dev/null

run_expect_fail "missing_file" ./parallel_search tests/invalid/does_not_exist.conf

for config in tests/invalid/*.conf; do
    run_expect_fail "$(basename "$config")" ./parallel_search "$config"
done

rm -f /tmp/parallel_search_invalid_stdout.txt
rm -f /tmp/parallel_search_invalid_stderr.txt

if [ "$failures" -ne 0 ]; then
    echo "$failures invalid configuration test(s) failed"
    exit 1
fi

echo "Invalid configuration tests passed"
