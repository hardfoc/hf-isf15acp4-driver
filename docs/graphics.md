# Graphics

How drawing works: the graphics context and its three backends, colours, text, images and what a frame costs.

Snippets on this page are compiled from [`tests/docs/graphics.cpp`](../tests/docs/graphics.cpp).

## The graphics context

`GraphicsContext` is the surface your code draws on. `display.CreateGraphicsContext()` returns one with the
display's variant and backend; you can also construct one directly with
`GraphicsContext(ProductVariant, GraphicsBackend)`.

| Call | Draws |
| --- | --- |
| `Clear()` | The whole frame |
| `SetPixel()`, `GetPixel()` | One pixel; out of range is ignored (reads black) |
| `DrawHLine()`, `DrawVLine()` | A row or column line, both ends included |
| `DrawRect()`, `FillRect()` | Outline or solid `Rect{x0, y0, x1, y1}`, inclusive |
| `DrawChar()`, `DrawString()` | 5 x 7 font on a background colour (default black) |
| `BlitRgb565()` | A row-major RGB565 image, clipped at the edge |

Every context holds a full 96 x 64 frame (12,288 bytes), so create one and keep it, rather than one per draw.

## Backends

`SmartDisplayConfig::graphics` picks how a context behaves. It is fixed when the context is created.

| Backend | Drawing | `Present()` |
| --- | --- | --- |
| `BuiltinCanvas` | Changes the frame in RAM | Uploads it |
| `Animator` | The same; pair it with the [`Animator` class](animations.md) | Uploads it |
| `HardwareDirect` | Shapes go to the display at once | Sends nothing |

`BuiltinCanvas` is the default.

### Hardware-direct drawing

```cpp
--8<-- "tests/docs/graphics.cpp:hardware-direct"
```

What to know before choosing it:

- **Bind the context you draw on.** `ConfigurePresenter(gfx)` connects that one context to the display. The
  binding `Initialize()` makes is on a temporary context and does not carry over.
- **Text and images stay in RAM.** `DrawChar()`, `DrawString()` and `BlitRgb565()` draw into the context's frame,
  which `Present()` never sends in this mode.
- **It saves no RAM.** The context still holds a 12,288-byte frame, and `Clear()` becomes `FillScreen()`, which
  builds a second frame on the stack.
- **Each pixel is a transaction.** `SetPixel()` costs 8 bytes and three SS-low transactions.

It suits a few lines and rectangles over whatever is on the screen. For anything with text, use the canvas.

## Colours

Colours are `Color565`: 16-bit RGB565, red in bits 15 to 11, green 10 to 5, blue 4 to 0.

| Helper | Use |
| --- | --- |
| `MakeColor565(r, g, b)` | From 8-bit channels |
| `colors::Red` ... | Ten presets |
| `BmpPixelToColor565()` | From a 24-bit BMP pixel (B, G, R) |

The presets are `Black`, `White`, `Red`, `Green`, `Blue`, `Yellow`, `Cyan`, `Magenta`, `Orange` and `Gray`.

The SSD1331 takes each pixel as two bytes, high byte first (65k-colour format, SSD1331 datasheet Table 9).
`Present()`, `DrawCanvas()`, `FillScreen()`, `DrawPixel()` and `WriteColors()` send that order on any CPU, so the
canvas stays in native byte order and needs no swapping.

The panel stores each pixel as blue, green, red (blue in bits 15 to 11, red in 4 to 0; NKK application notes,
section 9). The same calls convert every `Color565` to that order with `ssd1331::ToPanelPixel()`, and the
hardware line and rectangle commands send red as colour A and blue as colour C, so a colour looks the same however
it is drawn. Only `WritePixels()` sends bytes unchanged: give it panel-order words.

## Text

The font covers ASCII 32 (space) to 126 (`~`). Each character is 5 x 7 pixels with one column of spacing, so
a line holds 16 characters and the panel 8 lines at a 8-pixel pitch. Characters outside that range are skipped
(their cell is left as it was). Strings do not wrap; text past the right edge is clipped.

## Images

Convert a PNG to a C++ array with [`scripts/convert_image_to_rgb565.py`](../scripts/convert_image_to_rgb565.py)
(needs Pillow):

```bash
python3 scripts/convert_image_to_rgb565.py icon.png -o main/icon.hpp -n Icon -W 16 -H 16
```

It writes `kIconWidth`, `kIconHeight` and `kIconRgb565`, resizing the image to `-W` x `-H` (default 96 x 64).
Then blit it:

```cpp
--8<-- "tests/docs/graphics.cpp:image"
```

The panel's pixels are 1.32 times taller than wide, so a 96 x 64 image fills a 21.3 mm x 18.7 mm area. Draw
artwork at that shape (about 96 : 84) and let the script squeeze it to 96 x 64, and circles stay round.

## What a frame costs

| Operation | Bytes | At 6 MHz |
| --- | --- | --- |
| `Present()` | 12,294 | 16.4 ms |
| Filled rectangle | 17 | 23 µs |
| `DrawLine()` | 8 | 11 µs |
| `DrawPixel()` | 8 | 11 µs |

`FillScreen()` costs the same as `Present()`; `DrawPixel()` also takes three SS-low transactions.

Times are bytes x 8 / 6 MHz, before your port's per-transaction overhead and your drawing time. The SPI clock
limits full-frame animation to about 60 frames a second; 25 to 30 is a comfortable target.

Next: [Animations](animations.md) runs frames on a timer.
