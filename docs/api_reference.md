# API Reference

## Namespace `isf15acp4`

### Configuration

```cpp
struct SmartDisplayConfig {
    ProductVariant variant;       // default: Isf15acp4
    GraphicsBackend graphics;     // default: BuiltinCanvas
    bool enable_vcc_on_init;      // default: true
};
```

### `SmartDisplay<SpiType>`

| Method | Description |
|--------|-------------|
| `SmartDisplay(bus, config)` | Construct with variant + graphics backend |
| `Initialize(enable_vcc)` | Power-up, reset, SSD1331 init |
| `CreateGraphicsContext()` | Graphics surface matching config backend |
| `ConfigurePresenter(gfx)` | Bind hooks for HardwareDirect |
| `Present(gfx)` | Flush graphics context to panel |
| `DrawCanvas(canvas)` | Upload RGB565 framebuffer |
| `DisplayOn/Off/Dim()` | Display power states |
| `SetMasterCurrent(level)` | Global brightness 0x0–0xF |
| `SetWindow(rect)` | GRAM download window |
| `WritePixels(data, len)` | Raw RGB565 stream |
| `DrawPixel/Line/Rectangle/ClearWindow` | Mixed SW/HW drawing |
| `IsPressed()` | Momentary switch state |
| `ConfigureScroll(...)` | Screensaver scrolling |
| `GetGraphicsBackend()` | Active backend enum |
| `GetConfig()` | Full configuration struct |

### `GraphicsContext`

Unified drawing API — works with all backends.

| Method | Description |
|--------|-------------|
| `Clear(color)` | Fill surface |
| `SetPixel/GetPixel` | Pixel access |
| `DrawHLine/DrawVLine/DrawRect/FillRect` | Primitives |
| `DrawChar/DrawString` | 5×7 text |
| `BlitRgb565(...)` | Copy image region |
| `CanvasBuffer()` | Access underlying canvas |
| `BindPresenter(presenter)` | Hardware direct hooks |

### `GraphicsBackend`

| Value | Description |
|-------|-------------|
| `BuiltinCanvas` | CPU framebuffer (default) |
| `HardwareDirect` | Live SSD1331 commands |
| `Animator` | Canvas + frame sequencer |

### `Animator`

| Method | Description |
|--------|-------------|
| `Animator(gfx, fn, user, fps)` | Create sequencer |
| `Tick(elapsed_ms)` | Advance clock, render if due |
| `RenderNow()` | Force frame draw |
| `SetFps(fps)` / `Reset()` | Timing control |

### Built-in Animations (`isf15acp4::animations`)

| Function | Effect |
|----------|--------|
| `ColorWheel` | Rotating spectrum |
| `Plasma` | Sine interference |
| `ScrollingText` | Marquee (pass `ScrollTextState*`) |
| `PulseButton` | Press-reactive panel (pass `PulseButtonState*`) |

### `ButtonUI`

| Method | Description |
|--------|-------------|
| `Update(pressed, elapsed_ms)` | Returns `ButtonEvent` |
| `DrawStatus(gfx, label, active)` | Status panel |
| `DrawGauge(gfx, cx, cy, r, value, fg, bg)` | Arc gauge |
| `PressCount()` / `IsDown()` | State accessors |

### `ButtonEvent`

`None`, `Pressed`, `Released`, `Click`, `LongPress`

### Types

- `ProductVariant` — Isf15acp4, Isc15anp4, Isc01p
- `Color565`, `Rect`, `Point`, `DisplayGeometry`
- `Error` — driver error flags

## Doxygen

```bash
doxygen _config/Doxyfile
# Output: docs/doxygen/html/index.html
```

## Visual Guides

- [Architecture](architecture.md) — layer diagram
- [Graphics Engines](graphics_engines.md) — backend selection
- [Animations](animations.md) — example gallery
