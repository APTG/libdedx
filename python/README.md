# libdedx Python binding

This directory contains the `ctypes`-based Python binding for `libdedx`.

The Python module loads the native `libdedx` shared library at runtime.
Build and install the C library first, or set `LIBDEDX_SO` to the path of
the built shared library during development and testing.
