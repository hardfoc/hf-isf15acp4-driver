/**
 * @file api_tasks.cpp
 * @brief The API guide's and Configuration page's task snippets, compiled by the host build (never run).
 */
#include "my_spi.hpp"

#include <cstdint>
#include <cstdio>

using Display = isf15acp4::SmartDisplay<MySpi>;
namespace colors = isf15acp4::colors;

bool configure(MySpi& spi) {
  // --8<-- [start:configure]
  const isf15acp4::SmartDisplayConfig config{
      .variant = isf15acp4::ProductVariant::Isf15acp4,
      .graphics = isf15acp4::GraphicsBackend::BuiltinCanvas,
      .enable_vcc_on_init = true,
  };
  isf15acp4::SmartDisplay<MySpi> display(&spi, config);
  return display.Initialize(config.enable_vcc_on_init);
  // --8<-- [end:configure]
}

bool brightness(Display& display) {
  // --8<-- [start:brightness]
  if (!display.DisplayOff()) {                 // AEh: sleep, GRAM kept
    return false;
  }
  if (!display.SetMasterCurrent(0x08)) {       // 87h: 0x00 (dimmest) .. 0x0F (brightest)
    return false;
  }
  if (!display.DisplayOn()) {                  // AFh: normal display
    return false;
  }
  return display.DisplayDim();                 // ACh: dim mode, for an idle screen
  // --8<-- [end:brightness]
}

bool draw_frame(Display& display, uint32_t rpm) {
  // --8<-- [start:draw]
  auto gfx = display.CreateGraphicsContext();  // holds a 12,288-byte frame: keep it alive
  gfx.Clear(colors::Black);
  gfx.FillRect({0, 0, 95, 10}, colors::Blue);
  gfx.DrawString(2, 2, "SPINDLE", colors::White, colors::Blue);

  char text[16];
  std::snprintf(text, sizeof(text), "%lu RPM", static_cast<unsigned long>(rpm));
  gfx.DrawString(2, 24, text, colors::Cyan);

  return display.Present(gfx);                 // full-window upload
  // --8<-- [end:draw]
}

bool hardware_draw(Display& display) {
  // --8<-- [start:hardware]
  if (!display.ClearWindow({0, 0, 95, 63})) {                          // 25h
    return false;
  }
  if (!display.DrawRectangle({10, 10, 85, 53}, colors::Orange, true)) { // 26h fill on, 22h, fill off
    return false;
  }
  return display.DrawLine({0, 63}, {95, 0}, colors::White);            // 21h
  // --8<-- [end:hardware]
}

bool raw_pixels(Display& display) {
  // --8<-- [start:raw]
  // An 8 x 8 red square at (40, 28): window, then 64 pixels high byte first.
  uint8_t block[8 * 8 * 2];
  for (std::size_t i = 0; i < sizeof(block); i += 2) {
    block[i] = static_cast<uint8_t>(colors::Red >> 8);
    block[i + 1] = static_cast<uint8_t>(colors::Red & 0xFF);
  }
  if (!display.SetWindow({40, 28, 47, 35})) {   // 15h / 75h, inclusive bounds
    return false;
  }
  if (!display.WritePixels(block, sizeof(block))) {
    return false;
  }
  return display.SetFullWindow();               // back to 96 x 64 for Present()
  // --8<-- [end:raw]
}

bool scroll(Display& display) {
  // --8<-- [start:scroll]
  // Horizontal scroll of all 64 rows by 1 column, no vertical offset, fastest interval (code 0).
  return display.ConfigureScroll(1, 0, 64, 0, 0, true);  // 27h, then 2Fh; false sends 2Eh
  // --8<-- [end:scroll]
}

void button(Display& display, isf15acp4::ButtonUI& ui, uint32_t elapsed_ms) {
  // --8<-- [start:button]
  // Call every 10-20 ms from your loop; elapsed_ms is the time since the previous call.
  switch (ui.Update(display.IsPressed(), elapsed_ms)) {
    case isf15acp4::ButtonEvent::Pressed:   std::printf("down, press #%lu\n",
                                                        static_cast<unsigned long>(ui.PressCount())); break;
    case isf15acp4::ButtonEvent::Click:     std::printf("click\n"); break;
    case isf15acp4::ButtonEvent::LongPress: std::printf("held 800 ms\n"); break;
    case isf15acp4::ButtonEvent::Released:  std::printf("released after a long press\n"); break;
    case isf15acp4::ButtonEvent::None:      break;
  }
  // --8<-- [end:button]
}

void errors(Display& display) {
  // --8<-- [start:errors]
  if (!display.Initialize()) {
    switch (display.GetLastError()) {
      case isf15acp4::Error::NotInitialized: std::printf("port: EnsureInitialized() failed\n"); break;
      case isf15acp4::Error::SpiWrite:       std::printf("port: WriteBytes() failed\n"); break;
      case isf15acp4::Error::InvalidParam:   std::printf("no port (null bus pointer)\n"); break;
      default:                               std::printf("error 0x%04x\n", display.GetErrorFlags()); break;
    }
  }
  if (display.HasError(isf15acp4::Error::OutOfRange)) {   // flags accumulate until cleared
    display.ClearErrorFlags(static_cast<uint16_t>(isf15acp4::Error::OutOfRange));
  }
  std::printf("driver %s\n", Display::GetDriverVersion());
  // --8<-- [end:errors]
}
