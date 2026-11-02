# Configuration

## CMake options (standalone driver)

| Option | Default | Description |
|:-------|:--------|:------------|
| `HF_ISF15ACP4_ENABLE_WARNINGS` | `OFF` | Enable `-Wall -Wextra -Wpedantic` on the driver target |

Version and source lists are defined in `cmake/hf_isf15acp4_build_settings.cmake` (single source of truth).

## SmartDisplayConfig

Select product variant and graphics backend at construction time:

```cpp
isf15acp4::SmartDisplayConfig config{
    .variant = isf15acp4::ProductVariant::Isf15acp4,
    .graphics = isf15acp4::GraphicsBackend::BuiltinCanvas,
    .enable_vcc_on_init = true,
};
```

### Graphics backends

| Backend | Use when |
|:--------|:---------|
| `BuiltinCanvas` | Full framebuffer drawing, text, UI (default) |
| `HardwareDirect` | Minimal CPU RAM; use SSD1331 draw commands only |
| `Animator` | Procedural demos (color wheel, plasma, scroll, pulse) |

## ESP32 Kconfig defaults

Override pins in `sdkconfig` or `sdkconfig.defaults`:

| Symbol | Default | Pin |
|:-------|:--------|:----|
| `CONFIG_ISF15ACP4_SPI_HOST` | 1 | SPI2 |
| `CONFIG_ISF15ACP4_PIN_MOSI` | 11 | MOSI |
| `CONFIG_ISF15ACP4_PIN_SCLK` | 12 | SCK |
| `CONFIG_ISF15ACP4_PIN_CS` | 10 | Chip select (software) |
| `CONFIG_ISF15ACP4_PIN_DC` | 13 | Data/command |
| `CONFIG_ISF15ACP4_PIN_RES` | 14 | Reset |
| `CONFIG_ISF15ACP4_PIN_VCC_EN` | 9 | OLED supply enable |
| `CONFIG_ISF15ACP4_PIN_SWITCH` | 8 | Pushbutton input |

## hf-core integration

In the parent platform, enable explicitly (default is **OFF**):

```cmake
set(HF_CORE_ENABLE_ISF15ACP4 ON)
```

This defines `HARDFOC_ISF15ACP4_SUPPORT=1` and links `Isf15acp4Handler`.
