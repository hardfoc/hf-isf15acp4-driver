# Testing and CI

How the driver is checked: host unit tests against a recording port, compiled documentation, ESP32 builds and
static analysis, on every pull request.

## Host unit tests

The tests build the real driver against a port that records every SS-low transaction (D/C level and bytes), every
control-pin change and every delay, on a virtual clock. They check the exact SSD1331 byte stream, the reset and
VCC sequence, and the error flags, without hardware.

```bash
cmake -S . -B build-host -DHF_ISF15ACP4_BUILD_TESTS=ON
cmake --build build-host
ctest --test-dir build-host --output-on-failure
```

| File | Covers |
| --- | --- |
| [`tests/test_driver.cpp`](../tests/test_driver.cpp) | Init table and window, reset timing, VCC, port failures, range checks, pixel writes, hardware commands, frame upload, canvas, `ButtonUI`, `Animator` |
| [`tests/mock_spi.hpp`](../tests/mock_spi.hpp) | The recording port, with fault injection |
| [`tests/test_harness.hpp`](../tests/test_harness.hpp) | A dependency-free test registry and `CHECK` macros |

The display returns no data, so the tests check what the driver sends, not what the panel shows. Colours and
pixel order need a look at real hardware.

## Documentation snippets

Every C++ block on this site is included from [`tests/docs/`](../tests/docs/my_spi.hpp), which the host build
compiles (target `hf_isf15acp4_doc_snippets`) with the driver instantiated over the documented port. A change to
the API that breaks a page breaks the build first.

## CI workflows

| Workflow | What it checks |
| --- | --- |
| Host unit tests | GCC and Clang build and run the tests and the doc snippets |
| ESP32 examples build | Every app in `examples/esp32/app_config.yml`, for each listed ESP-IDF version and build type |
| C++ lint, C++ analysis | clang-format, clang-tidy and cppcheck |
| Docs | Strict site build, figure and generated-table drift, Doxygen, link check, Lighthouse budget |
| Markdown lint, YAML lint, link check | Repository text files |

The workflows call the shared, versioned workflows of
[hf-general-ci-tools](https://github.com/hardfoc/hf-general-ci-tools) and
[hf-espidf-ci-tools](https://github.com/hardfoc/hf-espidf-ci-tools).

## On hardware

The [ESP32 examples](examples.md) are the hardware test suite: `isf15acp4_comprehensive_test` runs every driver
call and prints a pass / fail summary over the serial monitor. It sees only the return values; check the panel
by eye.
