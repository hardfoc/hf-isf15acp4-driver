# Platform Integration

![ESP32 wiring reference](assets/isf15acp4-wiring-esp32.png)

## SpiInterface Checklist

| Method | Responsibility |
|--------|----------------|
| `EnsureInitialized()` | Configure SPI bus, GPIO directions, idle levels |
| `BeginCommand()` | D/C = command, CS active |
| `BeginData()` | D/C = data, CS active |
| `EndTransaction()` | CS inactive |
| `WriteBytes()` | Blocking SPI write while CS held |
| `GpioSet(Reset, Active)` | RES low (active-low reset) |
| `GpioSet(VccEnable, Active)` | Enable 16 V charge pump |
| `GpioRead(Switch)` | Return true when pressed |
| `DelayUs()` | Microsecond delays for reset pulse |

## Graphics Backend Setup

After transport works, select a graphics engine in `SmartDisplayConfig`:

```cpp
isf15acp4::SmartDisplayConfig config{
    .variant = isf15acp4::ProductVariant::Isf15acp4,
    .graphics = isf15acp4::GraphicsBackend::BuiltinCanvas,
};

isf15acp4::SmartDisplay display(&adapter, config);
display.Initialize(true);

auto gfx = display.CreateGraphicsContext();
display.ConfigurePresenter(gfx);  // only required for HardwareDirect
```

## ESP32 Adapter

The reference adapter is `examples/esp32/main/esp32_isf15acp4_spi_adapter.hpp`:

- SPI mode 0, software CS
- 6 MHz default clock (below 6.6 MHz datasheet max)
- Active-low switch with internal pull-up

Copy and adjust GPIO constants for your PCB.

## STM32 / Other MCUs

Implement the same `SpiInterface` contract:

1. Initialize SPI master (MOSI + SCK only — no hardware CS)
2. GPIO outputs for CS, D/C, RES, VCC enable
3. GPIO input for switch (with pull-up)
4. Honor NKK timing: complete bytes before releasing CS

## HardFOC hf-core Handler (planned)

Integration will bridge `BaseSpi` + `BaseGpio` to this CRTP interface, following `As5047uHandler` / `Tmc5160Handler` patterns.

## Pin Mapping (ISF15ACP4)

See [Hardware Notes](hardware.md) for the full pinout diagram and power design.
