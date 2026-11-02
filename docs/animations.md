# Animations

Run a frame callback at a fixed rate, use the four built-in effects, and draw button feedback with `ButtonUI`.

Snippets on this page are compiled from [`tests/docs/graphics.cpp`](../tests/docs/graphics.cpp).

## The animator

`Animator(gfx, frame_fn, user_data, fps)` calls `frame_fn(gfx, frame, user_data)` each time a frame is due. It
keeps no clock of its own: you tell it how much time has passed.

```cpp
--8<-- "tests/docs/graphics.cpp:animator"
```

| Member | Does |
| --- | --- |
| `Tick(elapsed_ms)` | Adds the time; when at least one frame period has built up, renders one frame and returns `true` |
| `RenderNow()` | Renders the next frame at once |
| `SetFps(fps)` | Frame period `1000 / fps` ms (33 ms when `fps` is 0) |
| `Frame()`, `Reset()` | The frame counter; `Reset()` sets it and the time to zero |

`Tick()` renders at most one frame per call and keeps any extra time, so after a late call the next calls catch
up one frame each; no frame number is skipped. The animator only draws: you call `Present()` when it returns
`true`.

## Built-in effects

| Function | Draws |
| --- | --- |
| `ColorWheel` | A rotating colour wheel, with a label |
| `Plasma` | A moving sine-interference pattern |
| `ScrollingText` | A title and a line of text moving left |
| `PulseButton` | A frame and a growing bar, green when pressed |

They are in `isf15acp4::animations`. `ColorWheel` and `Plasma` ignore `user_data`. `ScrollingText` takes an
`animations::ScrollTextState*` (its `text`; null shows "ISF15ACP4"), `PulseButton` an
`animations::PulseButtonState*` (its `pressed`; null is not pressed).

`ColorWheel` and `Plasma` compute every pixel with floating-point sine functions: on an MCU without an FPU that
can take longer than the frame period.

## Your own effect

A frame function is a plain `noexcept` function. Pass your state through `user_data`:

```cpp
--8<-- "tests/docs/graphics.cpp:custom"
```

The animator keeps a reference to the context, so the context must outlive it.

## Button feedback

`ButtonUI` debounces the switch ([API guide](api-guide.md#read-the-switch)) and draws two kinds of feedback:

```cpp
--8<-- "tests/docs/graphics.cpp:gauge"
```

| Call | Draws |
| --- | --- |
| `DrawStatus()` | A full status screen: title bar with the label, "PRESSED" or "READY", a border |
| `DrawGauge()` | A half disc below its centre, the first `value` (0 to 1) of it in `fg` |

`DrawStatus()` clears the frame to dark green when active, black otherwise. `DrawGauge()` is static.

The [examples](examples.md) combine these: `isf15acp4_animation_demo` cycles the four effects on each press, and
`isf15acp4_button_dashboard` draws a gauge driven by the switch.
