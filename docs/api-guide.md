# API guide

How to do the common jobs with the driver: initialise, draw, use the controller's drawing commands, set brightness,
read the switch and handle errors.

Every snippet on this page is compiled by the host CI from
[`tests/docs/api_tasks.cpp`](../tests/docs/api_tasks.cpp), with `Display` standing for
`isf15acp4::SmartDisplay<MySpi>` and `colors` for `isf15acp4::colors`. The [API reference](api-reference.md)
lists every member.

## The shape of the API

One `SmartDisplay<Port>` object per button. You draw into a `GraphicsContext` and hand it to the display; or you
call the display's own drawing and window functions, which talk to the controller directly.

| Task | Calls |
| --- | --- |
| Start | `Initialize()`, `Reset()`, `IsInitialized()` |
| Draw a frame | `CreateGraphicsContext()`, then `GraphicsContext` drawing, then `Present()` |
| Draw on the controller | `DrawLine()`, `DrawRectangle()`, `ClearWindow()`, `DrawPixel()`, `FillScreen()` |
| Write pixels yourself | `SetWindow()`, `SetFullWindow()`, `WritePixels()`, `DrawCanvas()` |
| Brightness and power | `DisplayOn()`, `DisplayOff()`, `DisplayDim()`, `SetMasterCurrent()` |
| Scroll | `ConfigureScroll()` |
| Switch | `IsPressed()`, and `ButtonUI` for events |
| Errors | `GetLastError()`, `GetErrorFlags()`, `HasError()`, `ClearErrorFlags()` |

Every call that talks to the display returns `bool` and never throws. Check it: `false` means a parameter was out
of range or the port reported a failure, and an `Error` flag says which.

## Initialise

Pick the product variant and graphics backend in `SmartDisplayConfig`, then call `Initialize()`:

```cpp
--8<-- "tests/docs/api_tasks.cpp:configure"
```

`Initialize()` brings the port up, pulses RES, enables VCC (unless you pass `false`), sends the variant's
initialisation table (which ends with display on) and selects the full window. `Reset()` runs the same sequence
again, for example after a supply brown-out. [Configuration](configuration.md) lists every field.

## Draw a frame

```cpp
--8<-- "tests/docs/api_tasks.cpp:draw"
```

Drawing into the context changes RAM on your MCU only; `Present()` sends the frame as one 12,288-byte data
transaction. At 6 MHz that is about 16 ms of SPI time, so a full frame costs at most about 60 frames a second
before your drawing time. Text uses the 5 x 7 font, 6 pixels per character: 16 characters across.
[Graphics](graphics.md) covers the backends, colours and images.

## Draw with the controller

The SSD1331 draws lines and rectangles itself from a few command bytes, which is much less SPI traffic than a frame:

```cpp
--8<-- "tests/docs/api_tasks.cpp:hardware"
```

| Call | Command | Bytes |
| --- | --- | --- |
| `DrawLine()` | `21h` | 8 |
| `DrawRectangle()` | `22h` | 11 |
| filled | `26h`, `22h`, `26h` | 17 |
| `ClearWindow()` | `25h` | 5 |
| `DrawPixel()` | `15h`, `75h`, data | 8 |
| `FillScreen()` | A full frame | 12,294 |

These draw straight into the display RAM. A later `Present()` overwrites them with the canvas contents.

## Write pixels yourself

```cpp
--8<-- "tests/docs/api_tasks.cpp:raw"
```

`SetWindow()` takes inclusive bounds and returns `false` with `Error::OutOfRange` outside 0..95, 0..63.
`WritePixels()` needs an even byte count and `Initialize()` first. Send each pixel high byte first, which is the
SSD1331's 65k-colour order. Restore the full window before the next `Present()`; `DrawCanvas()` and `Present()`
set it themselves.

## Brightness and power

```cpp
--8<-- "tests/docs/api_tasks.cpp:brightness"
```

| Call | Command | Effect |
| --- | --- | --- |
| `DisplayOn()` | `AFh` | Normal display |
| `DisplayOff()` | `AEh` | Sleep; the RAM keeps its contents |
| `DisplayDim()` | `ACh` | Dim mode, levels from the init table |
| `SetMasterCurrent()` | `87h` | Drive current scale, 0 to 15 |

The master current scales all three colours from 0 (dimmest) to 15 (brightest, the reset value).

Lower brightness also slows the OLED's ageing. `SetMasterCurrent()` rejects values above `0x0F` with
`Error::OutOfRange`.

## Scroll

```cpp
--8<-- "tests/docs/api_tasks.cpp:scroll"
```

`ConfigureScroll(h_offset, start_row, rows, v_offset, interval, activate)` sends the SSD1331 scroll setup (`27h`)
and then starts (`2Fh`) or stops (`2Eh`) scrolling. The interval code is 0 to 3: a step every 6, 10, 100 or 200
frames. The SSD1331 datasheet says not to change the scroll parameters while scrolling runs, and to rewrite the
display RAM after stopping it: call `Present()` (or redraw) after `ConfigureScroll(..., false)`.

## Read the switch

`IsPressed()` returns your port's `GpioRead(CtrlPin::Switch)`: the raw contact, with bounce. `ButtonUI` turns it
into events:

```cpp
--8<-- "tests/docs/api_tasks.cpp:button"
```

| Event | When |
| --- | --- |
| `Pressed` | The contact closed (counted in `PressCount()`) |
| `Click` | Released before the long-press time |
| `LongPress` | Held for the long-press time (default 800 ms); reported once per press |
| `Released` | Released after a long press |

`ButtonUI(debounce_ms, long_press_ms)` defaults to 25 ms and 800 ms. It samples the contact once per debounce
period, so call `Update()` more often than that.

## Handle errors

```cpp
--8<-- "tests/docs/api_tasks.cpp:errors"
```

| `Error::` | Set when |
| --- | --- |
| `SpiWrite` | Your `Begin*()` or `WriteBytes()` returned `false` |
| `NotInitialized` | `EnsureInitialized()` failed, or pixels before `Initialize()` |
| `InvalidParam` | No port, an odd pixel byte count, a wrong-size canvas |
| `OutOfRange` | A window off the panel, a master current above 15 |

`GetLastError()` is the most recent error; `GetErrorFlags()` is every error since the last `ClearErrorFlags()`.
`Error::GpioControl` and `Error::PowerSequence` exist in the enum but nothing sets them yet. The display sends
nothing back, so the driver cannot tell a missing or unpowered module from a working one:
[Troubleshooting](troubleshooting.md) starts there.
