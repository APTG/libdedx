# Contributing to libdedx

## C coding style

### Symbol naming and linkage

Use symbol names to make visibility obvious:

- Public API functions and types use the `dedx_*` prefix.
- Shared internal helpers used across multiple `.c` files use the `dedx_internal_*` prefix.
- File-local helpers should be declared `static` and use short unprefixed names.

Examples:

```c
/* public API */
float dedx_get_stp(dedx_workspace *ws, dedx_config *config, float energy, int *err);

/* shared internal helper */
float dedx_internal_get_atom_mass(int id, int *err);

/* file-local helper */
static int check_energy_bounds(dedx_internal_lookup_data *data, float energy);
```

Do not introduce new `_dedx_*` identifiers. Existing ones should be migrated to
the scheme above as code is touched.

### Variable declarations

Declare all variables at the top of their enclosing block, before any statements.
This is consistent with the Linux kernel coding style and makes the variable
inventory of a function visible before reading the logic.

```c
/* good */
int foo(int x) {
    const char *path;
    int result;
    FILE *fp;

    path = get_path();
    fp = fopen(path, "r");
    ...
}

/* bad — declaration mixed into statements */
int foo(int x) {
    const char *path = get_path();
    FILE *fp = fopen(path, "r");
    do_something();
    int result = 0;   /* not allowed */
    ...
}
```

Declarations at the top of an inner block (e.g. inside an `if` or `for`) are
fine when the variable is genuinely local to that scope.

## Thread safety

libdedx is currently **not thread-safe**. There is no synchronization around
workspace mutation in `dedx_load_config()` / the internal dataset loading
helpers. Do not share a `dedx_workspace` across threads without external
locking.

The intended fix is to audit the library for shared mutable state and either
make workspaces single-thread-owned by design or add explicit synchronization
where shared access is required. This is tracked as a known issue.

The Python binding inherits this limitation: do not share a `Workspace` across
threads. (Calls do not release the GIL, so binding calls on a shared workspace
are serialized in practice, but the underlying library is still not thread-safe.)

## Python binding

The Python package lives under `python/` and is a
[nanobind](https://nanobind.readthedocs.io) extension (`libdedx._core`) built by
[scikit-build-core](https://scikit-build-core.readthedocs.io). It statically
links the `dedx` C target, so building it also compiles the C library. The whole
project is configured from the top-level `pyproject.toml`.

```bash
pip install -e ".[dev]"   # builds the extension in place
pytest python/tests
ruff check .
```

Notes for contributors:

- The binding source (`python/src/dedx_core.cpp`) is C++17 and follows ordinary
  C++ conventions; the C "declare variables at the top of the block" rule above
  applies to the C library, not to this file.
- `dedx_config` owns the element arrays it is given and frees them in
  `dedx_free_config()`, so any pointer handed to it from the binding must be
  `malloc`'d. See the `Config` wrapper for how ownership and array lengths are
  kept consistent.
- The package version comes from `setuptools_scm` (git tags) and is fed into the
  C library so the two stay in lockstep.
