# Quick Start

Get the ISF15ACP4 SmartDisplay running in minutes.

![Wiring overview](assets/isf15acp4-wiring-esp32.png)

## 1. Wire the Hardware

Connect SPI (MOSI, SCK), GPIO (CS, D/C, RES), charge pump enable (VCC), and switch sense. See [Hardware Notes](hardware.md).

## 2. Implement Transport

```cpp
class MySpi : public isf15acp4::SpiInterface<MySpi> {
public:
    bool EnsureInitialized() noexcept { /* ... */ return true; }
    bool BeginCommand() noexcept { /* DC=0, CS=0 */ return true; }
    bool BeginData() noexcept { /* DC=1, CS=0 */ return true; }
    void EndTransaction() noexcept { /* CS=1 */ }
    bool WriteBytes(const uint8_t* data, std::size_t len) noexcept { /* SPI */ return true; }
    void GpioSet(isf15acp4::CtrlPin pin, isf15acp4::GpioSignal s) noexcept { /* RES, VCC */ }
    bool GpioRead(isf15acp4::CtrlPin pin) noexcept { return false; }
    void DelayUs(uint32_t us) noexcept { /* ... */ }
};
```

## 3. Create Display with Graphics Backend

```cpp
MySpi adapter;

isf15acp4::SmartDisplayConfig config{
    .variant = isf15acp4::ProductVariant::Isf15acp4,
    .graphics = isf15acp4::GraphicsBackend::BuiltinCanvas,
    .enable_vcc_on_init = true,
};

isf15acp4::SmartDisplay display(&adapter, config);
display.Initialize(true);
```

## 4. Draw and Present

```cpp
auto gfx = display.CreateGraphicsContext();
gfx.Clear(isf15acp4::colors::Black);
gfx.DrawString(8, 28, "HELLO", isf15acp4::colors::Cyan);
display.Present(gfx);
```

## 5. Read the Button

```cpp
if (display.IsPressed()) {
    // momentary contact closed
}
```

## ESP32 Fast Path

```bash
cd examples/esp32
git submodule update --init --recursive
./scripts/build_app.sh isf15acp4_button_dashboard Debug
./scripts/flash_app.sh isf15acp4_button_dashboard Debug
```

## Next Steps

- [Graphics Engines](graphics_engines.md) — switch between Canvas, Hardware, Animator
- [Animations](animations.md) — procedural effects and frame import
- [API Reference](api_reference.md) — complete method list
