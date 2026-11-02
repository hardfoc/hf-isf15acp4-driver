# Changelog

All notable changes to this driver are listed here. Versions follow
[Semantic Versioning](https://semver.org/). Entries marked **BREAKING** need source changes in
code that used the affected API; together they call for a major version bump at release.

## Unreleased

### Breaking changes

- **BREAKING** `CtrlPin::Reset` now means "held in reset": `Initialize()` drives it `Active` for at least 3 µs, then
  `Inactive`, and leaves it `Inactive` (SSD1331 rev 1.2 section 7.9, Table 18). Map `Active` to the low RES# level,
  like `ChipSelect`. Ports that followed the earlier workaround (`Active` = high) must invert their `Reset` mapping;
  the ESP32 example adapter already maps it this way.
- **BREAKING** `Present()`, `DrawCanvas()` and `FillScreen()` now send each pixel high byte first on every CPU
  (SSD1331 rev 1.2 section 7.5.2, Table 9). Remove any byte swap added around `Present()` as a workaround, or it
  swaps the frame back.
- **BREAKING** Pixel data (`Present()`, `DrawCanvas()`, `FillScreen()`, `DrawPixel()`, `WriteColors()`) is now
  converted from RGB565 to the panel's blue-green-red word, so red and blue are no longer swapped against the
  hardware line and rectangle commands (NKK Application Notes rev H section 9). Code that pre-swapped red and blue
  to compensate must stop. `WritePixels()` still sends raw bytes: convert them with `ssd1331::ToPanelPixel()`.

### Added

- `SmartDisplay::WriteColors(colors, count)` streams `Color565` pixels in the controller's byte order.
  `WritePixels()` still sends raw bytes unchanged.
- `SmartDisplay::StopScroll()` stops hardware scrolling (`2Eh`); `StopScroll(gfx)` also rewrites the display RAM
  with `Present(gfx)`, as the SSD1331 requires after `2Eh`.
- `ssd1331::ToPanelPixel(color)` converts a `Color565` to the panel's pixel word.

### Fixed

- The reset pulse left RES# low with an active-low port, so the controller stayed in reset. The driver now asserts
  RES# for at least 3 µs, releases it and waits 10 µs (reset completion is 2 µs maximum) before the first command.
- Frames went out in memory order, low byte first on little-endian MCUs (ESP32, Arm Cortex-M, x86), so colours
  were wrong after `Present()`. The canvas upload and the hardware-direct flush hook now serialise each pixel high
  byte first.
- Pixels drawn through the frame or `DrawPixel()` showed red as blue and blue as red: the NKK init tables select
  the normal colour order (A0h bit 2 = 0), in which the pixel word's high 5 bits are colour C, blue on these
  panels. `DrawLine()` and `DrawRectangle()` already sent red as colour A and were correct.
- `ProductVariant::Isc15anp4` and `Isc01p` failed `Initialize()` with `Error::OutOfRange`: `SetFullWindow()`
  passed RAM addresses (`10h`..`4Fh`, `16h`..`49h`) to `SetWindow()`, which checks display coordinates.
  `SetWindow()`, `DrawLine()`, `DrawRectangle()` and `ClearWindow()` now take display coordinates and add the
  variant's RAM offset (NKK Application Notes rev H, Tables 4 and 5).
- With `GraphicsBackend::HardwareDirect`, `Initialize()` bound the display to a temporary context, so contexts
  from `CreateGraphicsContext()` drew into RAM and nothing reached the panel. `CreateGraphicsContext()` on a
  non-const display now returns a bound context; a `const` display still returns an unbound one.
- `ConfigureScroll()` sent new `27h` parameters while scrolling could be running, and with `activate = false` sent
  `27h` before `2Eh`. It now always sends `2Eh` first, then `27h`, then `2Fh` only when activating (SSD1331 rev 1.2
  Table 10, `27h` note 2).
- `cmake --install` produced an unusable package: the headers were installed without the `src/*.ipp` files they
  include, and the exported target was `hf::hf_isf15acp4` instead of `hf::isf15acp4`. Headers now install to
  `include/hf_isf15acp4/inc/`, the `.ipp` files to `include/hf_isf15acp4/src/`, and `find_package(hf_isf15acp4)`
  provides `hf::isf15acp4`.

### Documentation

- The documentation site moved to the HardFOC MkDocs platform: new page set (quickstart, hardware, architecture,
  graphics, animations, API guide, configuration, porting, examples, troubleshooting), the Doxygen API reference
  under `/api/`, and every code sample compiled by the host build from `tests/docs/`.
- Corrected the hardware facts against the NKK datasheet: the ISC15ANP4 is 64 x 48 and the ISC01P 52 x 36 (not
  96 x 64 and 128 x 128), VCC is 14 V to 16 V (15 V typical), the power sequence raises VCC after the reset pulse,
  and the panel's pixels are 0.222 mm x 0.293 mm.
- Removed the illustrations whose pinout and wiring did not match the part.

### Added

- Host unit tests (`HF_ISF15ACP4_BUILD_TESTS`) that run the driver against a recording SPI port.
