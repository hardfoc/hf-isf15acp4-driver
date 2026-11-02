/**
 * @file quickstart.cpp
 * @brief The Quickstart page's example, compiled by the host build (never run).
 */
#include "my_spi.hpp"

// --8<-- [start:quickstart]
#include "isf15acp4.hpp"

int show_ready(MySpi& spi) {
  isf15acp4::SmartDisplay<MySpi> display(&spi);       // ISF15ACP4, software canvas

  if (!display.Initialize()) {                        // 1. reset, 15 V on, SSD1331 init table
    return -1;                                        // wiring, VCC or SPI: see GetLastError()
  }

  auto gfx = display.CreateGraphicsContext();         // 2. draw into a 96 x 64 RGB565 frame
  gfx.Clear(isf15acp4::colors::Black);
  gfx.DrawString(4, 4, "READY", isf15acp4::colors::Green);
  gfx.DrawRect({0, 0, 95, 63}, isf15acp4::colors::White);

  if (!display.Present(gfx)) {                        // 3. send the frame (12,288 bytes)
    return -2;
  }
  return display.IsPressed() ? 1 : 0;                 // 4. the switch, through your port
}
// --8<-- [end:quickstart]
