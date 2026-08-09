# AGENTS.md

Project brief for AI coding agents (Claude Code, opencode, Qwen Code, Codex, …).
This is the single source of truth; `CLAUDE.md` imports it. Keep it current when
build commands, conventions or architecture change.

## What this project is

`libdedx` is a C11 library for charged-particle **stopping power** (dE/dx)
calculations — the energy loss of a charged particle per unit length of a
material. All tabulated stopping-power data is **embedded into the library**;
no external `.bin` files are needed at runtime. A `ctypes`-based Python binding
lives in `python/`.

## Build, test, lint — the inner loop

C library (CMake 3.21+, C11 compiler):

```bash
cmake --preset debug                      # configure (also writes compile_commands.json)
cmake --build --preset debug --parallel   # build into ./build
ctest --preset debug                      # run the C test suite
```

Other presets: `release` (-> `build-release/`), `coverage` (-> `build-coverage/`).

Static analysis / formatting (both run in CI with warnings-as-errors):

```bash
clang-format -i <files>                   # style is enforced; see .clang-format
clang-tidy -p build <file.c>              # config in .clang-tidy, WarningsAsErrors: '*'
```

Memory checks (CI runs valgrind on the bethe test; do the same for memory-touching changes):

```bash
valgrind --leak-check=full --track-origins=yes --error-exitcode=1 ./build/tests/test_bethe_ext00
```

Python binding (`python/`):

```bash
pip install -e "python[dev]"              # editable install + ruff + pytest
export LIBDEDX_SO="$PWD/build/src/libdedx.so"   # point ctypes at the built .so
ruff check python                         # lint (line-length 120, rules E/F/W)
cd python && python3 -m pytest -q          # tests (use `-m pytest`, as CI does)
```

In Claude Code on the web, `.claude/hooks/session-start.sh` performs the
configure+build, the editable Python install, and exports `LIBDEDX_SO`
automatically, so the loop above is ready at session start.

## Repository map

| Path | What lives here |
|------|-----------------|
| `include/` | **Public API** headers (`dedx.h`, `dedx_error.h`, `dedx_tools.h`, `dedx_wrappers.h`, `dedx_elements.h`). The contract. |
| `src/` | Library implementation. Core engine in `dedx.c`; per-program models (`dedx_bethe.c`, `dedx_mstar.c`, `dedx_mpaul.c`); interpolation in `dedx_spline.c`; lookup/data access in `dedx_data_access.c`. |
| `src/data/embedded/` | **Generated** headers compiled into the library. Do not hand-edit. |
| `data/raw/` | Raw source tables + metadata, the regeneration inputs. |
| `tools/` | `dat2c.py` (raw tables -> embedded C headers) and `pdf2dat.py` (ICRU90 PDFs -> `.dat`). |
| `tests/` | CTest suite, one file per program/area (`test_pstar.c`, `test_bethe_ext00.c`, …). |
| `examples/` | Runnable usage samples; CI executes several of them. |
| `python/libdedx/` | ctypes binding (`_api.py`, typed stubs `__init__.pyi`, `py.typed`). |
| `docs/` | Sphinx + Doxygen sources; published to GitHub Pages. |

## API model (orient before editing)

- One-call convenience: `dedx_get_simple_stp(ion, target, energy, &err)`.
- Full path: allocate a `dedx_workspace`, fill a `dedx_config` (`program`, `ion`,
  `target`), `dedx_load_config()`, then `dedx_get_stp()`; free both.
- A result is selected by a `(program, ion, target)` triple. Programs include
  `DEDX_PSTAR`, `DEDX_ASTAR`, `DEDX_MSTAR`, `DEDX_ICRU49`, `DEDX_ICRU73`,
  `DEDX_ICRU73_OLD`, `DEDX_BETHE_EXT00`, and `DEDX_ICRU` (auto-selects the
  newest embedded ICRU data).
- Energies are in **MeV/nucl** throughout, including the generated data grids.

## Conventions (enforced — see CONTRIBUTING.md)

- **Naming/linkage:** public API `dedx_*`; shared internal helpers
  `dedx_internal_*`; file-local helpers `static` with short unprefixed names.
  Do **not** introduce new `_dedx_*` identifiers (leading underscore).
- **Declarations at top of block**, before any statements (Linux-kernel style).
  Inner-block declarations are fine when genuinely local to that scope.
- Formatting and clang-tidy are CI gates with warnings-as-errors — run them
  before claiming a change is done.

## Gotchas an agent will otherwise get wrong

- **Embedded data is generated, not authored.** To change tabulated values,
  edit `data/raw/` and regenerate, never edit `src/data/embedded/*` directly:
  ```bash
  python3 tools/pdf2dat.py        # refresh ICRU90 *.dat in data/raw/
  python3 tools/dat2c.py all      # regenerate src/data/embedded/ headers
  ```
- **Not thread-safe.** There is no synchronization around workspace mutation in
  `dedx_load_config()` / dataset loading. Never share a `dedx_workspace` across
  threads without external locking. Don't assume re-entrancy.
- **Compound fallback semantics.** For compound targets, some programs
  (`DEDX_PSTAR`, `DEDX_ASTAR`, `DEDX_MSTAR`) fall back to Bragg/stoichiometric
  weighting when the upstream database lacks that compound. A value returned
  under a program label is therefore not always a direct upstream value. Preserve
  this behavior and its caveats; don't "fix" it into looking like native coverage.
- **`ASTAR` vs `ICRU_ASTAR`** are distinct datasets (same grid/targets, slightly
  different values) — not duplicates. See `data/README.md`.

## Definition of done

1. `cmake --build --preset debug` is clean.
2. `ctest --preset debug` passes.
3. `clang-format` clean and `clang-tidy -p build` reports nothing (warnings = errors).
4. For memory-touching changes, valgrind is clean.
5. If Python is affected: `ruff check python` clean and `pytest` passes.

## Using a self-hosted Qwen (or other local model) via opencode

This repo is model-agnostic: Claude Code reads `CLAUDE.md` (which imports this
file); opencode and Qwen Code read this `AGENTS.md` directly. To drive the repo
with your own Qwen endpoint on custom hardware, add a custom provider to
opencode (`opencode.json` at the repo root or `~/.config/opencode/`). Point it at
your OpenAI-compatible server (vLLM / SGLang / Ollama / LM Studio) — model ids are
passed through unchanged, so they must match what your server exposes:

```jsonc
{
  "$schema": "https://opencode.ai/config.json",
  "provider": {
    "my-qwen": {
      "npm": "@ai-sdk/openai-compatible",
      "options": { "baseURL": "http://<your-host>:<port>/v1" },
      "models": { "qwen3-coder": {} }
    }
  }
}
```

Then select `my-qwen/qwen3-coder` via `/models`. (opencode defaults Qwen to
temperature 0.55 when unset.) This file is intentionally **not** committed so the
endpoint stays your local choice; the build/test/lint commands above are all the
context any agent needs regardless of the model behind it.
