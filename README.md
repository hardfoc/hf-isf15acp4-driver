# HF-ISF15ACP4 Driver

**The complete MCU-agnostic driver for the [NKK ISF15ACP4](https://www.nkkswitches.com/wp-content/themes/impress-blank/search/inc/part.php?part_no=ISF15ACP4) — a 96×64 RGB OLED pushbutton with integrated momentary switch.**

![ISF15ACP4 frameless OLED pushbutton](docs/assets/isf15acp4-product-hero.png)

## Why This Driver?

The ISF15ACP4 packs a vivid 65k-color OLED and a tactile button into a 23 mm footprint — ideal for machine panels, test fixtures, and compact HMI controls. This driver gives you:

- **Full SSD1331 protocol** — init, GRAM upload, hardware draw, scroll, dim, switch read
- **Swappable graphics engines** — `BuiltinCanvas`, `HardwareDirect`, `Animator` (select at construction)
- **ButtonUI helper** — debounced press/click/long-press with gauge and status rendering
- **Procedural animations** — color wheel, plasma, scrolling text, pulse effects
- **4 ESP32 examples** — test suite, UI demo, animation showcase, motor dashboard
- **Rich documentation** — pinout, wiring, and UI visual guides

## Quick Example

```cpp
isf15acp4::SmartDisplayConfig config{
    .variant = isf15acp4::ProductVariant::Isf15acp4,
    .graphics = isf15acp4::GraphicsBackend::Animator,
};

isf15acp4::SmartDisplay display(&adapter, config);
display.Initialize(true);

auto gfx = display.CreateGraphicsContext();
isf15acp4::Animator anim(gfx, isf15acp4::animations::ColorWheel, nullptr, 25);

while (true) {
    if (anim.Tick(33)) display.Present(gfx);
    if (display.IsPressed()) { /* your action */ }
}
```

## Graphics Backends

| Engine | Select With | Best For |
|--------|-------------|----------|
| **BuiltinCanvas** | `GraphicsBackend::BuiltinCanvas` | Screens, icons, imported images |
| **HardwareDirect** | `GraphicsBackend::HardwareDirect` | Low-RAM overlays, HW lines/rects |
| **Animator** | `GraphicsBackend::Animator` | Smooth procedural motion |

See [Graphics Engines](docs/graphics_engines.md).

## Example Gallery

| Preview | App |
|---------|-----|
| ![Dashboard mockup](docs/assets/isf15acp4-ui-dashboard-mockup.png) | `isf15acp4_button_dashboard` |
| ![Animation frame](docs/assets/isf15acp4-anim-frame-01.png) | `isf15acp4_animation_demo` |
| ![Button states](docs/assets/isf15acp4-button-states.png) | `isf15acp4_ui_demo` |

## Documentation

| Guide | Link |
|-------|------|
| Architecture | [docs/architecture.md](docs/architecture.md) |
| Configuration | [docs/configuration.md](docs/configuration.md) |
| CMake Integration | [docs/cmake_integration.md](docs/cmake_integration.md) |
| Hardware Setup | [docs/hardware_setup.md](docs/hardware_setup.md) |
| Graphics Engines | [docs/graphics_engines.md](docs/graphics_engines.md) |
| Animations | [docs/animations.md](docs/animations.md) |
| Examples | [docs/examples.md](docs/examples.md) |
| Troubleshooting | [docs/troubleshooting.md](docs/troubleshooting.md) |
| API Reference | [docs/api_reference.md](docs/api_reference.md) |
| ESP32 Examples | [examples/esp32/README.md](examples/esp32/README.md) |

## CI / Quality

| Workflow | Purpose |
|----------|---------|
| `esp32-examples-build-ci.yml` | ESP32 example matrix builds |
| `ci-cpp-lint.yml` | clang-format + clang-tidy |
| `ci-cpp-analysis.yml` | Cppcheck static analysis |
| `ci-markdown-lint.yml` | Markdown lint |
| `ci-yaml-lint.yml` | YAML lint |
| `ci-docs-linkcheck.yml` | Link validation |
| `ci-docs-publish.yml` | Jekyll + Doxygen to GitHub Pages |
| `release.yml` | Tagged release artifacts |

## hf-core integration

Submodule: `hf-core-drivers/external/hf-isf15acp4-driver`. Enable with `HF_CORE_ENABLE_ISF15ACP4 ON` (platform default **OFF**). See [docs/configuration.md](docs/configuration.md).

## ESP32 Build

```bash
cd examples/esp32
./scripts/build_app.sh isf15acp4_animation_demo Debug
./scripts/flash_app.sh isf15acp4_animation_demo Debug
```

## Features

- CRTP `SpiInterface` — zero virtual overhead, any MCU
- NKK Table 3 init for ISF15ACP4 (50k-hour frameless)
- RGB565 canvas + 5×7 font + PNG→C array converter
- `ButtonUI` — click, long-press, gauge, status panels
- `Animator` — timed frame callbacks at configurable FPS
- Doxygen-documented API
- CI matrix across ESP-IDF 5.4/5.5

## License

GNU General Public License v3.0 — see [LICENSE](LICENSE).
