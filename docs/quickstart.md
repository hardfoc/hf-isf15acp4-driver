# Quickstart

Show text on an ISF15ACP4 from your MCU: wire seven signals and two supplies, write a small port class, call four
driver functions.

!!! warning "The 15 V drive supply comes last"
    Switch VCC (14 V to 16 V) with a GPIO-controlled supply or charge pump that is off at power-up. The datasheet
    turns it on only after VDD is up and the reset pulse is done, and off before VDD. A VCC that is always on
    skips that sequence.

## 1. Wire it

| MCU | ISF15ACP4 pin | Notes |
| --- | --- | --- |
| SPI MOSI | 8 SDI | Write only: the display has no data output |
| SPI SCK | 7 SCK | SPI mode 0, up to 6.6 MHz; start at 1 MHz |
| GPIO out | 4 SS | Chip select, active low; the port drives it |
| GPIO out | 6 D/C | Low: command bytes; high: pixel data |
| GPIO out | 5 RES | Reset, active low |
| GPIO out | VCC supply enable | Turns on the 15 V supply that feeds pin 9 |
| GPIO in, pull-up | 1 SW | Pin 2 SW to GND: pressed reads low |
| 2.8 V or 3.3 V | 3 VDD | 2.4 V to 3.5 V; MCU I/O at the same level |
| GND | 10 GND | Common ground for both supplies |

The [Hardware](hardware.md) page has the timing, the power sequence and the electrical limits.

## 2. Add the driver

Add the repository as a submodule and link the `hf_isf15acp4` CMake target (or include `inc/` and compile
`src/isf15acp4_font_data.cpp` with C++20). [Installation](installation.md) covers CMake and ESP-IDF.

```bash
git submodule add https://github.com/hardfoc/hf-isf15acp4-driver.git external/hf-isf15acp4-driver
```

## 3. Write the port

The port is the only platform code. It writes SPI bytes inside an SS-low transaction with D/C set, drives the
control pins and waits. Replace the `platform_*` calls with your SDK's.

```cpp title="my_spi.hpp"
--8<-- "tests/docs/my_spi.hpp:port"
```

Map `CtrlPin::Reset` exactly as shown: in this release the driver ends its reset pulse with the pin `ACTIVE`, so
`ACTIVE` must be the high, running level. [Porting](porting.md#control-pins) explains each method and the pin
levels.

## 4. Show something

```cpp title="main.cpp"
--8<-- "tests/docs/quickstart.cpp:quickstart"
```

What each step does:

1. `Initialize()` calls your `EnsureInitialized()`, pulses RES for at least 3 µs, enables VCC, waits 1 ms, sends
   the ISF15ACP4 initialisation table (which ends with display on) and sets the full 96 x 64 window. Nothing
   else works until it returns `true`.
2. `CreateGraphicsContext()` returns a canvas the size of the panel. Drawing only changes RAM on your MCU.
3. `Present()` streams the frame to the display: 12,288 bytes with D/C high.
4. `IsPressed()` calls your `GpioRead(CtrlPin::Switch)`.

This example is built by the host CI, so it compiles against the current headers.

## If it does not work

| Symptom | First thing to check |
| --- | --- |
| `Initialize()` returns `false` | `GetLastError()`: your `EnsureInitialized()` or `WriteBytes()` failed |
| Returns `true`, screen stays dark | RES ends low (pin mapping), VCC not at 14 V to 16 V, SS or D/C swapped |
| Shapes in the right place, colours wrong | Byte order of the frame ([Troubleshooting](troubleshooting.md#colours-wrong-after-present)) |
| Switch always pressed or never | Pull-up on the SW pin, active level in `GpioRead()` |

## Next

- [API guide](api-guide.md): brightness, hardware drawing, raw pixel writes, the switch, errors.
- [Graphics](graphics.md): the three backends, text, images.
- [Examples](examples.md): complete ESP32 applications.
