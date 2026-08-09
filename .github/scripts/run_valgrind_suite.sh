#!/usr/bin/env bash
# Runs every test_* binary in the given directory under Valgrind, except the ones
# listed in SKIP_VALGRIND below. Used by the `valgrind` job in
# .github/workflows/ci.yml; also safe to run locally against a build directory,
# e.g. after `cmake -S . -B build && cmake --build build`:
#
#   .github/scripts/run_valgrind_suite.sh build/tests
#
set -euo pipefail

if [ "$#" -ne 1 ]; then
    echo "usage: $0 <test-binary-directory>" >&2
    exit 2
fi

test_dir="$1"

# Test binaries exempted from this script: they still run natively and under
# ASan+UBSan (see the `sanitize` CI job), just not here. Add a name below --
# don't special-case the loop below -- if another test earns an exemption.
SKIP_VALGRIND=(
    # ~102k load/query cycles: ~15s natively / ~25s under ASan+UBSan, but 15+
    # minutes under Valgrind's much heavier instrumentation, for close to zero
    # incremental memory-safety coverage over what the other test_* binaries
    # already give Valgrind on the same load/query code paths.
    test_availability_exhaustive
)

# Without nullglob, an unmatched glob expands to the literal pattern string; the
# -f/-x guard below would then just skip that one non-existent "file" and the loop
# would exit 0 having valgrinded nothing. nullglob plus the counter check below
# makes "no test binaries found" a hard failure instead of a silent pass.
shopt -s nullglob

ran=0
for bin in "$test_dir"/test_*; do
    name="$(basename "$bin")"
    if printf '%s\n' "${SKIP_VALGRIND[@]}" | grep -qx "$name"; then
        continue
    fi
    if [ -x "$bin" ] && [ -f "$bin" ]; then
        echo "::group::valgrind $bin"
        valgrind --leak-check=full --track-origins=yes --error-exitcode=1 "$bin"
        echo "::endgroup::"
        ran=$((ran + 1))
    fi
done

if [ "$ran" -eq 0 ]; then
    echo "::error::no test_* binaries found under $test_dir -- Valgrind ran nothing" >&2
    exit 1
fi
echo "Valgrind ran $ran test binaries (skipped: ${SKIP_VALGRIND[*]})"
