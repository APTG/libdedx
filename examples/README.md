# libdedx examples

Small, self-contained programs that show how to call libdedx. The C programs
live in this directory; the C++ programs (which consume the public headers
through their `extern "C"` guards) live in [`cpp/`](cpp/).

Every code block below is a **single command** — use the copy button and run
them one at a time, in order. All paths are relative to the repository root.

## Prerequisites

- CMake 3.21 or newer
- A C11 compiler (and a C++17 compiler for the `cpp/` examples)

## 1. Configure

```bash
cmake --preset debug
```

## 2. Build everything (library + examples)

```bash
cmake --build --preset debug
```

## 3. Run the C examples

```bash
./build/examples/dedx_example
```

```bash
./build/examples/dedx_list
```

```bash
./build/examples/dedx_use_wrappers
```

```bash
./build/examples/dedx_bethe
```

```bash
./build/examples/dedx_csda
```

```bash
./build/examples/dedx_custom_compound
```

## 4. Run the C++ examples

```bash
./build/examples/cpp/dedx_cpp_core
```

```bash
./build/examples/cpp/dedx_cpp_wrappers
```

## 5. Use the `getdedx` command-line tool

Look up PSTAR stopping power for a proton in water at 100 MeV/nucl:

```bash
./build/examples/getdedx 2 1 276 100
```

## Run examples as a test suite

Run every example test:

```bash
ctest --preset debug --output-on-failure
```

Run only the C++ example tests:

```bash
ctest --preset debug --output-on-failure -R dedx_cpp
```

## Building without the C++ examples

If you have no C++ compiler, turn the C++ examples off at configure time:

```bash
cmake --preset debug -DDEDX_BUILD_CXX_EXAMPLES=OFF
```

## Windows note

The Visual Studio generator is multi-config, so the binaries land in a
per-config subfolder. Build with an explicit config:

```bash
cmake --build build --config Release
```

Then run from the matching folder, e.g.:

```bash
./build/examples/cpp/Release/dedx_cpp_core.exe
```
