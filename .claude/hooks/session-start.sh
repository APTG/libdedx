#!/bin/bash
# SessionStart hook for libdedx.
#
# Prepares a fresh Claude Code on the web container so that builds, tests,
# linters and the Python bindings work immediately in-session:
#   - configures + builds the debug preset (produces compile_commands.json for
#     clang-tidy and build/src/libdedx.so for the Python ctypes binding)
#   - installs the Python package in editable mode with dev dependencies
#   - exports LIBDEDX_SO so `pytest` under python/ can find the shared library
#
# Safe to run multiple times. Only runs in the remote (web) environment.
set -euo pipefail

# Only set up the toolchain in Claude Code on the web; local checkouts are
# assumed to already have a working dev environment.
if [ "${CLAUDE_CODE_REMOTE:-}" != "true" ]; then
    exit 0
fi

cd "${CLAUDE_PROJECT_DIR:-$(pwd)}"

echo "[session-start] Configuring debug preset..."
cmake --preset debug

echo "[session-start] Building debug preset..."
cmake --build --preset debug --parallel

# Editable install of the ctypes binding plus dev tools (ruff, pytest).
# Ubuntu's system Python is externally managed (PEP 668); fall back to
# --break-system-packages when a normal editable install is refused.
echo "[session-start] Installing Python package (editable, with dev deps)..."
python3 -m pip install -e "python[dev]" \
    || python3 -m pip install --break-system-packages -e "python[dev]"

# Point the Python test suite at the freshly built shared library so that
# `pytest` works without any manual setup.
LIBDEDX_SO="${CLAUDE_PROJECT_DIR:-$(pwd)}/build/src/libdedx.so"
export LIBDEDX_SO
if [ -n "${CLAUDE_ENV_FILE:-}" ]; then
    echo "export LIBDEDX_SO=\"${LIBDEDX_SO}\"" >> "$CLAUDE_ENV_FILE"
fi

echo "[session-start] Done. Build, tests (ctest), clang-tidy and pytest are ready."
