---
hide:
  - navigation
  - toc
---

<div class="hf-hero" markdown>
<span class="hf-kicker">HardFOC driver</span>

# ISF15ACP4 Driver

<p>A C++20 driver for the NKK ISF15ACP4 SmartDisplay, a pushbutton with a 96 x 64 colour OLED in its keytop. It
initialises the display's SSD1331 controller, draws text, shapes, images and animations, and reads the switch, on
any MCU with SPI.</p>

[Quickstart](quickstart.md){ .md-button .md-button--primary }
[API guide](api-guide.md){ .md-button }
[Source on GitHub](https://github.com/hardfoc/hf-isf15acp4-driver){ .md-button }
</div>

The driver has no platform code. You give it a small class that writes SPI bytes with the D/C and SS lines, sets
the reset and supply-enable pins and reads the switch; it does the rest: the reset pulse, the controller's
initialisation table, memory windows, colour conversion and the hardware drawing commands.

<div class="hf-chip" markdown>
<div class="hf-chip__head">
<span class="hf-chip__kicker">Frameless OLED pushbutton</span>
<span class="hf-chip__name">ISF15ACP4</span>
<span class="hf-chip__vendor">NKK Switches (SmartDisplay)</span>
</div>

Display
:   96 x 64 RGB passive-matrix OLED, 65,536 colours (RGB565), driver with 96 x 64 x 16-bit display RAM

Host interface
:   SPI, write only: SS, D/C, SCK, SDI; clock cycle 150 ns minimum (about 6.6 MHz)

Supplies
:   VDD 2.4 V to 3.5 V (logic), VCC 14 V to 16 V (OLED drive, 11 mA typical with all pixels on)

Switch
:   SPST normally open, 100 mA at 12 V DC, 2.0 N, 1.8 mm travel, 1,000,000 operations

Operating range
:   −20 °C to +70 °C; display life 50,000 h at 100 cd/m² with 40 % of pixels on

Driver mode
:   65k-colour mode; full frames from a CPU canvas, or the controller's line and rectangle commands

[Datasheet (NKK)](https://www.nkkswitches.com/pdf/IS15ACP4_FramelessOLED.pdf)
</div>

## What you get

- **One object per button.** `SmartDisplay<YourPort>` owns the reset, the supply sequence and the SSD1331
  initialisation. See [API guide](api-guide.md).
- **A canvas with text.** `GraphicsContext` draws pixels, lines, rectangles, a 5 x 7 font and RGB565 images into a
  12,288-byte frame that `Present()` uploads. See [Graphics](graphics.md).
- **Hardware drawing.** Lines, filled rectangles, window clear and scrolling run on the controller with a few
  command bytes each. See [API guide](api-guide.md#draw-with-the-controller).
- **Animations and a button helper.** `Animator` runs a frame callback at a fixed rate; `ButtonUI` debounces the
  switch into press, click and long-press events. See [Animations](animations.md).
- **Zero-overhead portability.** The port is a CRTP class: no virtual calls, no heap, no RTOS dependency.
  See [Porting](porting.md).
- **Tested without hardware.** Host unit tests check the exact byte stream and pin sequence against a recording
  port. See [Testing and CI](testing.md).

## Where to go next

<div class="grid cards" markdown>

-   :material-rocket-launch-outline: **[Quickstart](quickstart.md)**

    ---

    Wire the button, write the port, show text in about 15 lines.

-   :material-chip: **[Hardware](hardware.md)**

    ---

    Pins, the two supplies, SPI timing and the power sequence.

-   :material-layers-outline: **[Architecture](architecture.md)**

    ---

    The three layers and what each one owns.

-   :material-code-braces: **[API guide](api-guide.md)**

    ---

    Initialise, draw, set brightness, read the switch and handle errors.

-   :material-swap-horizontal: **[Porting](porting.md)**

    ---

    Implement the SPI interface for your MCU.

-   :material-book-open-variant: **[API reference](api-reference.md)**

    ---

    Every class and member, generated from the headers.

</div>
