/**
 * @file graphics.cpp
 * @brief The Graphics and Animations pages' snippets, compiled by the host build (never run).
 */
#include "my_spi.hpp"

#include "isf15acp4_animator.hpp"

#include <cstddef>
#include <cstdint>

using Display = isf15acp4::SmartDisplay<MySpi>;
namespace colors = isf15acp4::colors;

bool hardware_direct(MySpi& spi) {
  // --8<-- [start:hardware-direct]
  isf15acp4::SmartDisplay<MySpi> display(
      &spi, isf15acp4::SmartDisplayConfig{.graphics = isf15acp4::GraphicsBackend::HardwareDirect});
  if (!display.Initialize()) {
    return false;
  }
  auto gfx = display.CreateGraphicsContext();
  display.ConfigurePresenter(gfx);                        // bind THIS context to the display

  gfx.FillRect({8, 8, 87, 55}, colors::Blue);             // sent at once as 26h / 22h
  gfx.DrawHLine(8, 87, 31, colors::White);                // sent at once as 21h
  return display.Present(gfx);                            // nothing to do: returns true
  // --8<-- [end:hardware-direct]
}

// --8<-- [start:image]
// 16 x 16 RGB565 icon, row-major, from scripts/convert_image_to_rgb565.py.
extern const isf15acp4::Color565 kIconRgb565[16 * 16];

bool show_icon(Display& display, isf15acp4::GraphicsContext& gfx) {
  gfx.Clear(colors::Black);
  gfx.BlitRgb565(40, 24, 16, 16, kIconRgb565);            // clipped at the frame edge
  return display.Present(gfx);
}
// --8<-- [end:image]

// --8<-- [start:animator]
void run_plasma(Display& display, uint32_t (*millis)()) {
  auto gfx = display.CreateGraphicsContext();
  isf15acp4::Animator anim(gfx, isf15acp4::animations::Plasma, nullptr, 25);  // 25 FPS: 40 ms per frame

  uint32_t last = millis();
  for (;;) {
    const uint32_t now = millis();
    if (anim.Tick(now - last)) {                          // renders when a frame is due
      display.Present(gfx);
    }
    last = now;
  }
}
// --8<-- [end:animator]

// --8<-- [start:custom]
struct Bar {
  uint8_t level;                                          // 0..95, updated by your code
};

void DrawBar(isf15acp4::GraphicsContext& gfx, uint32_t frame, void* user_data) noexcept {
  const auto* bar = static_cast<const Bar*>(user_data);
  gfx.Clear(colors::Black);
  gfx.FillRect({0, 28, bar->level, 36}, (frame / 15) % 2 ? colors::Green : colors::Yellow);
}

void start_bar(isf15acp4::GraphicsContext& gfx, Bar& bar) {
  isf15acp4::Animator anim(gfx, &DrawBar, &bar, 30);
  anim.RenderNow();                                       // first frame without waiting
}
// --8<-- [end:custom]

void dashboard(Display& display, isf15acp4::GraphicsContext& gfx, float load) {
  // --8<-- [start:gauge]
  isf15acp4::ButtonUI ui;
  ui.DrawStatus(gfx, "START", display.IsPressed());       // title bar, state text, frame
  isf15acp4::ButtonUI::DrawGauge(gfx, 48, 40, 20, load,   // half-circle gauge, load in 0..1
                                 colors::Green, colors::Gray);
  display.Present(gfx);
  // --8<-- [end:gauge]
}
