# Documentation Index

Welcome to the HF-ISF15ACP4 driver documentation.

![ISF15ACP4 SmartDisplay](assets/isf15acp4-product-hero.png)

## Getting Started

| Guide | Description |
|-------|-------------|
| [Quick Start](quickstart.md) | Minimal integration in 5 minutes |
| [Getting Started](getting_started.md) | Full walkthrough with visuals |
| [Installation](installation.md) | CMake, ESP-IDF, header-only copy |
| [Configuration](configuration.md) | CMake flags, Kconfig, hf-core opt-in |
| [CMake Integration](cmake_integration.md) | Standalone, ESP-IDF, hf-core |
| [Platform Integration](platform_integration.md) | Implement `SpiInterface` for your MCU |

## Core Concepts

| Guide | Description |
|-------|-------------|
| [Architecture](architecture.md) | Layered design with visual overview |
| [Hardware Setup](hardware_setup.md) | Pinout, power, wiring, variants |
| [Hardware Notes](hardware.md) | Supplemental electrical notes |
| [Graphics Engines](graphics_engines.md) | Swappable BuiltinCanvas / HardwareDirect / Animator |
| [Graphics Guide](graphics.md) | Colors, text, images, performance |
| [Animations](animations.md) | Procedural effects, example apps, frame import |

## Reference

| Guide | Description |
|-------|-------------|
| [API Reference](api_reference.md) | Method tables + Doxygen link |
| [Datasheet](datasheet/index.md) | Manufacturer links and PDF index |
| [Examples](examples.md) | ESP32 apps and CI matrix |
| [Troubleshooting](troubleshooting.md) | Common bring-up issues |

## Example Apps

| App | Description |
|-----|-------------|
| [ESP32 Examples](../examples/esp32/README.md) | Build, flash, monitor |
| `isf15acp4_comprehensive_test` | Full API test suite |
| `isf15acp4_animation_demo` | Color wheel, plasma, scroll, pulse |
| `isf15acp4_button_dashboard` | Gauge + debounced motor UI |
| `isf15acp4_ui_demo` | Press counter + live bar |

## Assets

Documentation images live in [`docs/assets/`](assets/). Convert custom PNGs with `scripts/convert_image_to_rgb565.py`.
