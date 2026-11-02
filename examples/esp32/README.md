# ESP32 ISF15ACP4 Examples

Four example applications demonstrate the full driver — from API validation to animated, button-driven UIs.

## Apps

| App | Category | Description |
|-----|----------|-------------|
| `isf15acp4_comprehensive_test` | test | Exercises every driver API path |
| `isf15acp4_ui_demo` | demo | Press counter + animated progress bar |
| `isf15acp4_animation_demo` | demo | Color wheel, plasma, scroll, pulse — press to cycle |
| `isf15acp4_button_dashboard` | demo | Motor RPM gauge, debounced click/long-press |

## Setup

```bash
git submodule update --init --recursive
```

## Build

```bash
./scripts/build_app.sh list
./scripts/build_app.sh isf15acp4_button_dashboard Debug
./scripts/build_app.sh isf15acp4_animation_demo Release
```

## Flash & Monitor

```bash
./scripts/flash_app.sh isf15acp4_button_dashboard Debug
./scripts/idf_flash_monitor.sh isf15acp4_button_dashboard Debug
```

## Graphics Backend per Example

| App | `GraphicsBackend` |
|-----|-------------------|
| comprehensive_test | `BuiltinCanvas` |
| ui_demo | `BuiltinCanvas` |
| animation_demo | `Animator` |
| button_dashboard | `BuiltinCanvas` + `ButtonUI` |

## Default GPIO Map

| Signal | GPIO |
|--------|------|
| MOSI | 11 |
| SCK | 12 |
| CS | 10 |
| D/C | 13 |
| RES | 14 |
| VCC EN | 9 |
| SWITCH | 8 |

Override in `sdkconfig` or example source headers.

## Import Custom Artwork

```bash
python ../../scripts/convert_image_to_rgb565.py my_icon.png -o main/my_icon.hpp -n MyIcon
```

Blit in your app:

```cpp
gfx.BlitRgb565(0, 0, kMyIconWidth, kMyIconHeight, kMyIconRgb565);
display.Present(gfx);
```

## Further Reading

- [Animations](../../docs/animations.md)
- [Graphics](../../docs/graphics.md)
- [Hardware](../../docs/hardware.md)
