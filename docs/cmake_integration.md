# CMake Integration

## Standalone `find_package`

```cmake
find_package(hf_isf15acp4 REQUIRED)
target_link_libraries(my_firmware PRIVATE hf::isf15acp4)
```

Install the driver or add it as a subdirectory:

```cmake
add_subdirectory(path/to/hf-isf15acp4-driver)
target_link_libraries(my_firmware PRIVATE hf::isf15acp4)
```

## ESP-IDF component wrapper

Copy `examples/esp32/components/hf_isf15acp4` or point `EXTRA_COMPONENT_DIRS` at the driver root.

The wrapper includes `cmake/hf_isf15acp4_build_settings.cmake` and registers:

- **Sources:** `src/isf15acp4_font_data.cpp`
- **Includes:** `inc/` + generated `isf15acp4_version.h`
- **Requires:** `driver`, `esp_driver_spi`, `freertos`

## hf-core monorepo

Submodule path: `hf-core-drivers/external/hf-isf15acp4-driver`

```cmake
# cmake/hf_core_build_settings.cmake (default OFF)
set(HF_CORE_ENABLE_ISF15ACP4 ON)   # opt-in only
```

When enabled, hf-core adds:

- Handler: `handlers/isf15acp4/Isf15acp4Handler.cpp`
- Driver sources and includes from `hf_isf15acp4_build_settings.cmake`
- Compile definition: `HARDFOC_ISF15ACP4_SUPPORT=1`

## Header-only / copy integration

Minimum files:

1. All headers under `inc/`
2. `src/isf15acp4_font_data.cpp`
3. Your `SpiInterface` implementation (see `examples/esp32/main/esp32_isf15acp4_spi_adapter.hpp`)

Add `inc/` to include path and compile the font data translation unit with C++20.
