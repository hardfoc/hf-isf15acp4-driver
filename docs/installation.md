# Installation

Add the driver to a CMake or ESP-IDF project. It is a header-mostly C++20 library with one source file and no
dependencies.

## Requirements

- A C++20 compiler: GCC 10+, Clang 12+ or a current MSVC. The headers use designated initialisers.
- No RTOS, heap or exceptions: every call is `noexcept` and returns `bool` or a value.
- About 12.3 KB of RAM per graphics context, and 13 KB of stack if you call `FillScreen()`
  ([Architecture](architecture.md#memory)).

## Get the source

A git submodule keeps the driver pinned to a version you chose:

```bash
git submodule add https://github.com/hardfoc/hf-isf15acp4-driver.git external/hf-isf15acp4-driver
git submodule update --init external/hf-isf15acp4-driver
```

Only `inc/`, `src/`, `cmake/` and `CMakeLists.txt` are needed to build. The examples have their own submodule
(`examples/esp32/scripts`); you do not need it to use the driver.

## CMake

```cmake
add_subdirectory(external/hf-isf15acp4-driver)
target_link_libraries(my_app PRIVATE hf_isf15acp4)
```

The `hf_isf15acp4` target (alias `hf::isf15acp4`) carries the include directories, including the generated
version header, and requires C++20 (`cxx_std_20`), so your target does not need to set either.

Both options default to `OFF`:

| Option | Effect when `ON` |
| --- | --- |
| `HF_ISF15ACP4_ENABLE_WARNINGS` | `-Wall -Wextra -Wpedantic` on the driver |
| `HF_ISF15ACP4_BUILD_TESTS` | The host tests in `tests/` ([Testing and CI](testing.md)) |

The root `CMakeLists.txt` also has `install()` rules and a `hf_isf15acp4Config.cmake` package. The installed
headers do not include `src/*.ipp`, which they need, so use `add_subdirectory()` until that is fixed.

## ESP-IDF

The ESP-IDF component wrapper is [`examples/esp32/components/hf_isf15acp4/`](../examples/esp32/components/hf_isf15acp4/CMakeLists.txt).
Copy that folder into your project's `components/` (or point `EXTRA_COMPONENT_DIRS` at it) and set the
`HF_ISF15ACP4_ROOT` cache variable to the driver checkout; the wrapper stops with a clear message when the path is
wrong. Your component then declares `REQUIRES hf_isf15acp4`. The wrapper also brings the example's Kconfig menu
([Configuration](configuration.md#esp32-example-kconfig)).

## Without a build system

Add `inc/` to the include path and compile `src/isf15acp4_font_data.cpp`. The `.ipp` files in `src/` are included
by the headers. Generate `isf15acp4_version.h` from `inc/isf15acp4_version.h.in` (CMake does this at configure
time), or provide your own with the four `HF_ISF15ACP4_VERSION_*` macros.

## How the build is organised

| Layer | Role |
| --- | --- |
| [Settings](../cmake/hf_isf15acp4_build_settings.cmake) | Sources, include directories, version: the single source of truth |
| [Library](../CMakeLists.txt) | Static library for host builds and CI |
| [ESP-IDF](../examples/esp32/components/hf_isf15acp4/CMakeLists.txt) | Component wrapper over the same settings |

The version is set once in the settings file (`cmake/hf_isf15acp4_build_settings.cmake`) and written into
`isf15acp4_version.h` at configure time. Read it at run time:

```cpp
const char* v = isf15acp4::SmartDisplay<MySpi>::GetDriverVersion();   // "1.0.0"
```

## Next

[Quickstart](quickstart.md) wires the button and shows text; [Porting](porting.md) writes the SPI interface for
your MCU.
