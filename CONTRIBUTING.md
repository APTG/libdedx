# Contributing to libdedx

## C coding style

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
the static path cache in `_dedx_get_data_path()`, nor around workspace mutation
in `dedx_load_config()` / `_dedx_load_data()`. Do not share a `dedx_workspace`
across threads without external locking.

The intended fix is to replace the `done` flag in `_dedx_get_data_path()` with
C11 `call_once()`, and audit the rest of the library for shared mutable state.
This is tracked as a known issue.
