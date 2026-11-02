# Configuration

Every setting the driver takes: the construction-time configuration, the build options and the ESP32 example's
Kconfig.

## SmartDisplayConfig

Pass it to the constructor; it is fixed for the life of the `SmartDisplay` object.

```cpp
--8<-- "tests/docs/api_tasks.cpp:configure"
```

| Field | Default | Meaning |
| --- | --- | --- |
| `variant` | `Isf15acp4` | Init table, geometry and window ([Hardware](hardware.md#product-variants)) |
| `graphics` | `BuiltinCanvas` | Backend of new graphics contexts ([Graphics](graphics.md)) |
| `enable_vcc_on_init` | `true` | Stored only: pass it to `Initialize()` yourself |

The two-argument constructor `SmartDisplay(bus, variant)` uses the defaults for the rest.

## Initialisation table

`Initialize()` sends the table for the variant from [`inc/isf15acp4_ssd1331.hpp`](../inc/isf15acp4_ssd1331.hpp).
For the ISF15ACP4:

<div class="hf-registers" markdown>

| Command | Data | Sets |
| --- | --- | --- |
| `81h`, `82h`, `83h` | `1Ch`, `2Eh`, `2Ch` | Contrast of colours A, B, C |
| `8Ah`, `8Bh`, `8Ch` | `4Fh`, `74h`, `88h` | Second pre-charge speed of A, B, C |
| `87h` | `0Fh` | Master current: maximum |
| `A0h` | `68h` | Remap: 65k colours, COM split and scan direction |
| `A1h`, `A2h` | `00h`, `00h` | Display start line, display offset |
| `A4h` | | Normal display |
| `A8h` | `3Fh` | Multiplex ratio: 64 rows |
| `ABh` | `00h 0Eh 17h 16h 12h` | Dim mode contrast and pre-charge |
| `ADh` | `8Eh` | Master configuration: external VCC |
| `B0h` | `0Bh` | Power save off |
| `B1h` | `17h` | Phase 1 and 2 periods |
| `B3h` | `F0h` | Clock divider and oscillator frequency |
| `B9h` | | Linear grey-scale table |
| `BBh` | `1Dh` | Pre-charge level |
| `BEh` | `20h` | V<sub>COMH</sub> level |
| `AFh` | | Display on |

</div>

To change a value, for example the contrast, send the command after `Initialize()` through your own transaction
(`BeginCommand()`, `WriteBytes()`, `EndTransaction()` on your port), or edit the table. The values come from NKK's
programming guidance for this part; test any change on the panel.

## CMake options

Both options default to `OFF`:

| Option | Effect when `ON` |
| --- | --- |
| `HF_ISF15ACP4_ENABLE_WARNINGS` | `-Wall -Wextra -Wpedantic` on the driver |
| `HF_ISF15ACP4_BUILD_TESTS` | The host tests and the documentation snippets ([Testing and CI](testing.md)) |

## ESP32 example Kconfig

The examples read their pins from `menuconfig` (**HF ISF15ACP4 SmartDisplay**), defined in
[`Kconfig`](../examples/esp32/components/hf_isf15acp4/Kconfig). Every symbol starts with `CONFIG_ISF15ACP4_`:

| Symbol | Default | Signal |
| --- | --- | --- |
| `SPI_HOST` | `1` | SPI2 (`2` is SPI3) |
| `PIN_MOSI` | `11` | SDI |
| `PIN_SCLK` | `12` | SCK |
| `PIN_CS` | `10` | SS, driven as a GPIO |
| `PIN_DC` | `13` | D/C |
| `PIN_RES` | `14` | RES |
| `PIN_VCC_EN` | `9` | Enable of the VCC supply |
| `PIN_SWITCH` | `8` | SW, pull-up, pressed reads low |

The SPI clock is `ISF15ACP4_SPI_CLOCK_HZ` (6 MHz) in
[`esp32_isf15acp4_test_config.hpp`](../examples/esp32/main/esp32_isf15acp4_test_config.hpp).

Next: [Porting](porting.md) writes the port for your MCU.
