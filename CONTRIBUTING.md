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
