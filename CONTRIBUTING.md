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
static int check_energy_bounds(_dedx_lookup_data *data, float energy);
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
the static path cache in the file-local data-path helper in
[`libdedx/dedx_file_access.c`](libdedx/dedx_file_access.c),
nor around workspace mutation in `dedx_load_config()` / the internal dataset
loading helpers. Do not share a `dedx_workspace`
across threads without external locking.

The intended fix is to replace the cached-path `done` flag with C11
`call_once()`, and audit the rest of the library for shared mutable state. This
is tracked as a known issue.
