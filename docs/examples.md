# Examples

Complete ESP32-S3 applications that exercise the driver on an ISF15ACP4, from an API test suite to an animated,
button-driven dashboard.

They run on an ESP32-S3 with the pins from the Kconfig menu ([Configuration](configuration.md#esp32-example-kconfig))
at 6 MHz; the port is [`esp32_isf15acp4_spi_adapter.hpp`](../examples/esp32/main/esp32_isf15acp4_spi_adapter.hpp)
and the set-up helpers are in [`esp32_isf15acp4_setup.hpp`](../examples/esp32/main/esp32_isf15acp4_setup.hpp).
CI builds every app below on each pull request.

## The apps

<!-- generated: examples -->
| App | What it shows | Source | CI |
| --- | --- | --- | --- |
| `isf15acp4_comprehensive_test` :material-star:{ title="featured" } | Comprehensive ISF15ACP4 driver validation suite | [`isf15acp4_comprehensive_test.cpp`](../examples/esp32/main/isf15acp4_comprehensive_test.cpp) | yes |
| `isf15acp4_ui_demo` :material-star:{ title="featured" } | Interactive SmartDisplay UI demo with button counter | [`isf15acp4_ui_demo.cpp`](../examples/esp32/main/isf15acp4_ui_demo.cpp) | yes |
| `isf15acp4_animation_demo` :material-star:{ title="featured" } | Procedural animation showcase (color wheel, plasma, scroll, pulse) | [`isf15acp4_animation_demo.cpp`](../examples/esp32/main/isf15acp4_animation_demo.cpp) | yes |
| `isf15acp4_button_dashboard` :material-star:{ title="featured" } | Motor-control dashboard with gauge, debounced button, RPM simulation | [`isf15acp4_button_dashboard.cpp`](../examples/esp32/main/isf15acp4_button_dashboard.cpp) | yes |

Build and flash one (from `examples/esp32`, ESP-IDF environment loaded):

```bash
./scripts/build_app.sh isf15acp4_comprehensive_test Release
./scripts/flash_app.sh flash_monitor isf15acp4_comprehensive_test Release
```

Source of this table: [`examples/esp32/app_config.yml`](../examples/esp32/app_config.yml).
<!-- /generated -->

## Set up once

```bash
git submodule update --init examples/esp32/scripts   # the HardFOC ESP-IDF build scripts
cd examples/esp32
. $IDF_PATH/export.sh                                 # ESP-IDF v5.5
./scripts/build_app.sh list                           # every app and build type
```

The scripts read [`app_config.yml`](../examples/esp32/app_config.yml) (the same file this page's table comes
from) and build each app as its own firmware image.

## What to read in an example

| You want to | Read |
| --- | --- |
| See every driver call exercised, with pass / fail output | `isf15acp4_comprehensive_test.cpp` |
| Count presses and draw a progress bar | `isf15acp4_ui_demo.cpp` |
| Run the four built-in effects, switching on each press | `isf15acp4_animation_demo.cpp` |
| Combine `ButtonUI` events with a gauge | `isf15acp4_button_dashboard.cpp` |
| Write an ESP-IDF port | `esp32_isf15acp4_spi_adapter.hpp` |

The [example folder's README](../examples/esp32/README.md) has the GPIO map and the build commands.
