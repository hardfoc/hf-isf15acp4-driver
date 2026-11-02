# Getting Started with ISF15ACP4

The ISF15ACP4 combines a **96×64 color OLED** and a **momentary pushbutton** in one through-hole package — perfect as a compact "do something" control for motors, scripts, test rigs, and panel UIs.

![Product photo](assets/isf15acp4-product-hero.png)

## What You Get

| Capability | Driver Support |
|------------|----------------|
| 65k-color display | RGB565 canvas + SPI upload |
| Tactile button | `IsPressed()`, `ButtonUI` debouncing |
| Animated UIs | `Animator` + 4 built-in effects |
| Static icons/labels | PNG → RGB565 converter |
| Low-RAM overlays | `HardwareDirect` backend |
| Multi-variant panels | ISF15ACP4, ISC15ANP4, ISC01P |

## 5-Minute Integration

### Step 1 — Transport

Implement `isf15acp4::SpiInterface<YourAdapter>` for your MCU SPI + GPIO.

### Step 2 — Configure

```cpp
isf15acp4::SmartDisplayConfig config{
    .variant = isf15acp4::ProductVariant::Isf15acp4,
    .graphics = isf15acp4::GraphicsBackend::BuiltinCanvas,
    .enable_vcc_on_init = true,
};
```

### Step 3 — Initialize

```cpp
isf15acp4::SmartDisplay display(&adapter, config);
display.Initialize(true);
```

### Step 4 — Draw + Present

```cpp
auto gfx = display.CreateGraphicsContext();
gfx.DrawString(10, 28, "START", isf15acp4::colors::Green);
display.Present(gfx);
```

### Step 5 — React to Press

```cpp
if (display.IsPressed()) {
    gfx.FillRect({20, 40, 76, 56}, isf15acp4::colors::Orange);
    display.Present(gfx);
}
```

## Choose Your Graphics Engine

![Architecture layers](assets/isf15acp4-architecture-layers.png)

| If you need… | Use |
|--------------|-----|
| Full screens & text | `BuiltinCanvas` |
| Smooth motion | `Animator` |
| Minimal RAM | `HardwareDirect` |

Details: [Graphics Engines](graphics_engines.md)

## Try the Examples

On ESP32 with the included scripts:

```bash
cd examples/esp32
./scripts/build_app.sh isf15acp4_button_dashboard Debug
./scripts/flash_app.sh isf15acp4_button_dashboard Debug
```

![Dashboard mockup](assets/isf15acp4-ui-dashboard-mockup.png)

## Learn More

- [Hardware & Wiring](hardware.md)
- [Animations](animations.md)
- [API Reference](api_reference.md)
