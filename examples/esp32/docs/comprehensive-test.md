# ISF15ACP4 Comprehensive Test

**App:** `isf15acp4_comprehensive_test`  
**Source:** `main/isf15acp4_comprehensive_test.cpp`

## Purpose

Validates the full driver stack on real hardware:

- SPI/GPIO adapter initialization
- SSD1331 init and display power sequencing
- `BuiltinCanvas` graphics present path
- Hardware draw commands (line, rectangle)
- Display dim/on/off
- Integrated pushbutton readback
- Full-frame refresh timing

## Hardware

Wire the ISF15ACP4 per [../../../docs/hardware.md](../../../docs/hardware.md). The default GPIO map is in
`main/esp32_isf15acp4_test_config.hpp` (overridable via `menuconfig` → **HF ISF15ACP4 SmartDisplay**).

## Build

```bash
./scripts/build_app.sh isf15acp4_comprehensive_test Debug
./scripts/flash_app.sh flash_monitor isf15acp4_comprehensive_test Debug
```

## Expected Output

Serial log shows `PASS:` lines for each stage, then `=== TEST SUMMARY ===`. With hardware connected, the OLED
cycles blue/red, draws text and shapes, then enters a live button indicator loop.

## CI

`ci_enabled: true` in `app_config.yml` — included in the ESP32 build matrix.
