# Hardware Notes

> **Primary reference:** See [Hardware Setup](hardware_setup.md) for pinout, power sequencing, wiring, and supported product variants.

Supplemental electrical and mechanical notes for the NKK SmartDisplay family.

## ISF15ACP4 at a Glance

![Product hero — frameless OLED pushbutton](assets/isf15acp4-product-hero.png)

| Spec | Value |
|------|-------|
| Display | 96 × 64 RGB OLED, passive matrix |
| Colors | 65,536 (RGB565) |
| Controller | SSD1331 |
| Interface | SPI — SS, D/C, SCK, SDI + RES |
| Switch | SPST-NO momentary, 100 mA @ 12 V DC |
| Logic supply | VDD 2.4–3.5 V |
| Drive supply | VCC 14–16 V (typ. 15 V) |
| Display life | 50,000 h @ 40% pixels on |

## Pinout

![Annotated pinout diagram](assets/isf15acp4-pinout-diagram.png)

| Pin | Signal | Function |
|-----|--------|----------|
| 1–2 | SW | Momentary switch (normally open) |
| 3 | VDD | Logic power |
| 4 | SS | Chip select (active low) |
| 5 | RES | Reset (active low, ≥ 3 µs) |
| 6 | D/C | Command (low) / Data (high) |
| 7 | SCK | SPI clock (max 6.6 MHz) |
| 8 | SDI | SPI MOSI |
| 9 | VCC | OLED drive (~16 V) |
| 10 | GND | Ground |

## ESP32 Wiring

![ESP32 to ISF15ACP4 wiring](assets/isf15acp4-wiring-esp32.png)

Default example GPIO map (override per board):

| Signal | ESP32 GPIO |
|--------|------------|
| MOSI | 11 |
| SCK | 12 |
| CS | 10 |
| D/C | 13 |
| RES | 14 |
| VCC EN | 9 |
| SWITCH | 8 (pull-up, active low) |

## Power Design

Two rails are mandatory:

1. **VDD** — powers SSD1331 logic (2.4–3.5 V)
2. **VCC** — OLED panel drive (~16 V via charge pump)

NKK charge pump modules:

- **IS-CHPMP** — 21 mA @ 16 V
- **IS-CHPMPHP** — 100 mA @ 16 V

**Critical rules:**

- VCC **disabled** at power-up
- VCC enabled only after reset sequence
- VCC must **never** fall below VDD
- Keep VCC capacitance ≤ VDD capacitance

## Layout Recommendations

- 0.1 µF ceramic at VDD pin
- 22–27 Ω series resistor on SCK near the switch
- Series resistors on other signal lines for noise immunity
- Solid ground plane under the socket

## Socket & Prototyping

- Socket: **AT9704-085L**
- Engineering kits: IS-ENG-KIT-7-FF (dual ISF15ACP4)

## Image Geometry

Pixels are **not square** on ISF15ACP4. NKK recommends **75% vertical compression** when converting desktop bitmaps.

Full frame: **12,288 bytes** (96 × 64 × 2).

## References

- [ISF15ACP4 product page](https://www.nkkswitches.com/wp-content/themes/impress-blank/search/inc/part.php?part_no=ISF15ACP4)
- [NKK OLED Application Notes](https://docs.nkkswitches.com/docs/application-notes/oled-application-notes/)
- Frameless OLED datasheet: `IS15ACP4_FramelessOLED.pdf`
