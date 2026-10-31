---
layout: default
title: Installation
parent: Documentation
nav_order: 12
---
# Installation

## CMake (recommended)

```bash
git clone https://github.com/hardfoc/hf-isf15acp4-driver.git
cd hf-isf15acp4-driver
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix /path/to/install
```

Consume from another CMake project:

```cmake
find_package(hf_isf15acp4 REQUIRED)
target_link_libraries(my_app PRIVATE hf::isf15acp4)
```

## Header + Source Copy

Minimum files:

- `inc/*.hpp`
- `src/isf15acp4_font_data.cpp`
- `src/*.ipp` (included by headers)

Add `inc/` to your include path and compile `isf15acp4_font_data.cpp`.

## ESP-IDF Component

Use `examples/esp32/components/hf_isf15acp4` as a reference wrapper, or add the driver root to `EXTRA_COMPONENT_DIRS`.

## Requirements

- C++20 compiler
- No external graphics libraries (built-in Canvas + 5×7 font)
- Platform SPI master + 4 GPIO outputs (CS, D/C, RES, VCC enable optional)
