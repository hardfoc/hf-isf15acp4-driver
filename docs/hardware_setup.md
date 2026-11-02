# Hardware Setup

> **Supplemental notes:** See [Hardware Notes](hardware.md) for additional electrical context.

![Pinout reference](assets/isf15acp4-pinout-diagram.png)

## Interface summary

| Signal | Description |
|:-------|:------------|
| MOSI / SCK | SPI mode 0, up to ~6.6 MHz typical |
| CS (SS) | Software chip select — active low |
| D/C | Low = command, high = pixel data |
| RES | Active-low reset |
| VCC_EN | Enables OLED high-voltage supply (~16 V via module charge pump) |
| SWITCH | Momentary pushbutton (active low on ISF15ACP4) |

## Power sequencing

1. Assert RES (hold in reset)
2. Enable VCC_EN and wait for charge pump (`~50–100 ms` in driver init)
3. Release RES and send SSD1331 init table for selected `ProductVariant`
4. Turn display on and upload framebuffer or use HW draw commands

## ESP32 wiring

![ESP32 wiring](assets/isf15acp4-wiring-esp32.png)

Default example GPIO map is documented in [Configuration](configuration.md). Use a dedicated SPI bus or share MOSI/SCK with other devices; **CS, D/C, RES, and VCC_EN must be unique per module**.

## Supported variants

| Variant | Resolution | Notes |
|:--------|:-----------|:------|
| `Isf15acp4` | 96×64 | Frameless pushbutton (primary target) |
| `Isc15anp4` | 96×64 | Alternate NKK series |
| `Isc01p` | 128×128 | Larger panel init table |

## Socket / mechanical

Mount the module in the NKK SmartDisplay socket per manufacturer drawing. Keep flex and pressure within datasheet limits; excessive force can damage the OLED bond.
