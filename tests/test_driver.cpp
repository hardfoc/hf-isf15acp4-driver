/**
 * @file test_driver.cpp
 * @brief SmartDisplay, Canvas, ButtonUI and Animator against the recording SPI port.
 */
#include "isf15acp4.hpp"
#include "isf15acp4_animator.hpp"
#include "mock_spi.hpp"
#include "test_harness.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

using isf15acp4::CtrlPin;
using isf15acp4::GpioSignal;
using Display = isf15acp4::SmartDisplay<isf15acp4_test::MockSpi>;

namespace {

std::vector<uint8_t> bytes(std::initializer_list<uint8_t> list) { return list; }

}  // namespace

TEST_CASE("Initialize sends the ISF15ACP4 table, then the full 96x64 window") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize());
  CHECK(display.IsInitialized());

  std::size_t count = 0;
  const auto* table = isf15acp4::ssd1331::GetInitTableIsf15acp4(count);
  const auto cmds = spi.commands();
  REQUIRE(cmds.size() == count + 2);
  for (std::size_t i = 0; i < count; ++i) {
    CHECK_EQ(cmds[i][0], table[i].command);
    CHECK_EQ(cmds[i].size(), std::size_t{1} + table[i].data_len);
  }
  CHECK(cmds[0] == bytes({0x81, 0x1C}));
  CHECK(cmds[count - 1] == bytes({0xAF}));       // display on, last step of the table
  CHECK(cmds[count] == bytes({0x15, 0x00, 0x5F}));     // columns 0..95
  CHECK(cmds[count + 1] == bytes({0x75, 0x00, 0x3F})); // rows 0..63
  CHECK_EQ(spi.open_transactions, 0);
}

TEST_CASE("Every command and its parameters go in one CS-low transaction") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize());
  for (const auto& t : spi.transactions) {
    CHECK(!t.data);
    CHECK(!t.bytes.empty());
  }
}

TEST_CASE("Reset asserts RES# for at least 3 us, releases it, and waits before the first command") {
  // SSD1331 rev 1.2 section 7.9 and Table 18: RES# low >= 3 us, then high; reset completes within 2 us.
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize(true));

  std::vector<isf15acp4_test::PinEvent> reset;
  const isf15acp4_test::PinEvent* vcc_on = nullptr;
  for (const auto& e : spi.pins) {
    if (e.pin == CtrlPin::Reset) {
      reset.push_back(e);
    }
    if (e.pin == CtrlPin::VccEnable && e.signal == GpioSignal::Active) {
      vcc_on = &e;
    }
  }
  REQUIRE(reset.size() == 2);
  REQUIRE(vcc_on != nullptr);
  CHECK(reset[0].signal == GpioSignal::Active);    // RES# low: chip held in reset
  CHECK(reset[1].signal == GpioSignal::Inactive);  // RES# high: chip released and left running
  CHECK(reset[1].time_us - reset[0].time_us >= isf15acp4::ssd1331::kResetPulseMinUs);
  CHECK(vcc_on->time_us >= reset[0].time_us + 3);  // t2: VCC at least 3 us after RES# went low
  CHECK(vcc_on->time_us >= reset[1].time_us + 2);  // after the reset completion time
}

TEST_CASE("Reset() leaves RES# released") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize());
  spi.clear();
  REQUIRE(display.Reset());
  const isf15acp4_test::PinEvent* last = nullptr;
  for (const auto& e : spi.pins) {
    if (e.pin == CtrlPin::Reset) {
      last = &e;
    }
  }
  REQUIRE(last != nullptr);
  CHECK(last->signal == GpioSignal::Inactive);
}

TEST_CASE("Initialize(false) never enables VCC") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize(false));
  for (const auto& e : spi.pins) {
    CHECK(!(e.pin == CtrlPin::VccEnable && e.signal == GpioSignal::Active));
  }
}

TEST_CASE("A port that fails to start or to write is reported") {
  {
    isf15acp4_test::MockSpi spi;
    spi.init_ok = false;
    Display display(&spi);
    CHECK(!display.Initialize());
    CHECK(display.HasError(isf15acp4::Error::NotInitialized));
  }
  {
    isf15acp4_test::MockSpi spi;
    spi.write_ok = false;
    Display display(&spi);
    CHECK(!display.Initialize());
    CHECK(display.HasError(isf15acp4::Error::SpiWrite));
    CHECK(display.GetLastError() == isf15acp4::Error::SpiWrite);
    CHECK_EQ(spi.open_transactions, 0);
    display.ClearErrorFlags();
    CHECK_EQ(display.GetErrorFlags(), 0u);
  }
}

TEST_CASE("Pixel writes need Initialize() and an even byte count") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  const uint8_t px[2] = {0xF8, 0x00};
  CHECK(!display.WritePixels(px, 2));
  CHECK(display.HasError(isf15acp4::Error::NotInitialized));

  REQUIRE(display.Initialize());
  CHECK(!display.WritePixels(px, 1));
  CHECK(display.HasError(isf15acp4::Error::InvalidParam));
  CHECK(display.WritePixels(px, 2));
}

TEST_CASE("SetWindow and SetMasterCurrent reject out-of-range values without sending") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize());
  spi.clear();
  CHECK(!display.SetWindow({0, 0, 96, 10}));
  CHECK(!display.SetMasterCurrent(0x10));
  CHECK(display.HasError(isf15acp4::Error::OutOfRange));
  CHECK(spi.transactions.empty());

  CHECK(display.SetMasterCurrent(0x0F));
  CHECK(spi.commands().back() == bytes({0x87, 0x0F}));
}

TEST_CASE("DrawPixel sets a 1x1 window and sends the colour high byte first") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize());
  spi.clear();
  REQUIRE(display.DrawPixel(10, 20, 0xF81F));
  REQUIRE(spi.transactions.size() == 3);
  CHECK(spi.transactions[0].bytes == bytes({0x15, 10, 10}));
  CHECK(spi.transactions[1].bytes == bytes({0x75, 20, 20}));
  CHECK(spi.transactions[2].data);
  CHECK(spi.transactions[2].bytes == bytes({0xF8, 0x1F}));
}

TEST_CASE("Hardware line and filled rectangle send their geometry and colour bytes") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize());
  spi.clear();
  REQUIRE(display.DrawLine({0, 0}, {95, 63}, isf15acp4::colors::Green));
  // 21h: x0 y0 x1 y1, then colours C, B, A on a 6-bit scale (green is B, 6 bits).
  CHECK(spi.commands().back() == bytes({0x21, 0, 0, 95, 63, 0x00, 0x3F, 0x00}));

  spi.clear();
  REQUIRE(display.DrawRectangle({1, 2, 30, 40}, isf15acp4::colors::Green, true));
  const auto cmds = spi.commands();
  REQUIRE(cmds.size() == 3);
  CHECK(cmds[0] == bytes({0x26, 0x01, 0x00}));  // fill on
  CHECK(cmds[1] == bytes({0x22, 1, 2, 30, 40, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00}));
  CHECK(cmds[2] == bytes({0x26, 0x00, 0x00}));  // fill off again
}

TEST_CASE("Present uploads the whole 12288-byte frame") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize());
  auto gfx = display.CreateGraphicsContext();
  gfx.Clear(isf15acp4::colors::Blue);
  spi.clear();
  REQUIRE(display.Present(gfx));
  REQUIRE(!spi.transactions.empty());
  CHECK(spi.transactions.back().data);
  CHECK_EQ(spi.transactions.back().bytes.size(), std::size_t{12288});
}

TEST_CASE("Present sends every pixel high byte first, whatever the CPU byte order") {
  // SSD1331 rev 1.2 section 7.5.2, Table 9: in 65k colour the first byte of a pixel is its high byte.
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  REQUIRE(display.Initialize());
  auto gfx = display.CreateGraphicsContext();
  gfx.Clear(isf15acp4::colors::White);
  gfx.SetPixel(0, 0, 0xFFE0);   // first pixel in the frame
  gfx.SetPixel(95, 63, 0xFFE0);  // last pixel
  spi.clear();
  REQUIRE(display.Present(gfx));
  const auto& frame = spi.transactions.back().bytes;
  REQUIRE(frame.size() == 12288);
  CHECK(frame[0] == 0xFF);
  CHECK(frame[1] == 0xE0);
  CHECK(frame[12286] == 0xFF);
  CHECK(frame[12287] == 0xE0);
  CHECK_EQ(spi.open_transactions, 0);
}

TEST_CASE("WriteColors needs Initialize() and a buffer, and uses one data transaction") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  const isf15acp4::Color565 px[3] = {0xFFE0, 0x07FF, 0xFFFF};
  CHECK(!display.WriteColors(px, 3));
  CHECK(display.HasError(isf15acp4::Error::NotInitialized));
  REQUIRE(display.Initialize());
  CHECK(!display.WriteColors(nullptr, 3));
  CHECK(!display.WriteColors(px, 0));
  spi.clear();
  REQUIRE(display.WriteColors(px, 3));
  REQUIRE(spi.transactions.size() == 1);
  CHECK(spi.transactions[0].data);
  CHECK(spi.transactions[0].bytes == bytes({0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0xFF}));
}

TEST_CASE("IsPressed reads the Switch pin through the port") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  CHECK(!display.IsPressed());
  spi.switch_pressed = true;
  CHECK(display.IsPressed());
}

TEST_CASE("Colours and canvas drawing") {
  CHECK_EQ(isf15acp4::MakeColor565(255, 255, 255), 0xFFFFu);
  CHECK_EQ(isf15acp4::MakeColor565(255, 0, 0), 0xF800u);
  CHECK_EQ(isf15acp4::MakeColor565(0, 255, 0), 0x07E0u);
  CHECK_EQ(isf15acp4::MakeColor565(0, 0, 255), 0x001Fu);
  const uint8_t bgr[3] = {0xFF, 0x00, 0x00};
  CHECK_EQ(isf15acp4::BmpPixelToColor565(bgr), 0x001Fu);

  isf15acp4::Canvas canvas;
  CHECK_EQ(canvas.Width(), 96);
  CHECK_EQ(canvas.Height(), 64);
  CHECK_EQ(canvas.ByteSize(), std::size_t{12288});
  canvas.SetPixel(95, 63, isf15acp4::colors::White);
  canvas.SetPixel(96, 0, isf15acp4::colors::White);  // out of range: ignored
  CHECK_EQ(canvas.GetPixel(95, 63), 0xFFFFu);
  CHECK_EQ(canvas.GetPixel(200, 0), 0u);

  canvas.Clear(isf15acp4::colors::Black);
  canvas.DrawString(0, 0, "I", isf15acp4::colors::White);
  int lit = 0;
  for (uint8_t y = 0; y < 7; ++y) {
    for (uint8_t x = 0; x < 6; ++x) {
      lit += canvas.GetPixel(x, y) == isf15acp4::colors::White ? 1 : 0;
    }
  }
  CHECK(lit > 0);
}

TEST_CASE("ButtonUI debounces, counts presses and reports click and long press") {
  isf15acp4::ButtonUI button(25, 800);
  CHECK(button.Update(true, 10) == isf15acp4::ButtonEvent::None);  // inside the debounce time
  CHECK(button.Update(true, 20) == isf15acp4::ButtonEvent::Pressed);
  CHECK(button.IsDown());
  CHECK_EQ(button.PressCount(), 1u);
  CHECK(button.Update(false, 30) == isf15acp4::ButtonEvent::Click);

  CHECK(button.Update(true, 30) == isf15acp4::ButtonEvent::Pressed);
  isf15acp4::ButtonEvent ev = isf15acp4::ButtonEvent::None;
  for (int i = 0; i < 40 && ev != isf15acp4::ButtonEvent::LongPress; ++i) {
    ev = button.Update(true, 30);
  }
  CHECK(ev == isf15acp4::ButtonEvent::LongPress);
  CHECK(button.Update(false, 30) == isf15acp4::ButtonEvent::Released);
  CHECK_EQ(button.PressCount(), 2u);
}

namespace {
int g_frames = 0;
void CountFrame(isf15acp4::GraphicsContext&, uint32_t, void*) noexcept { ++g_frames; }
}  // namespace

TEST_CASE("Animator renders one frame per period") {
  isf15acp4::GraphicsContext gfx(isf15acp4::ProductVariant::Isf15acp4,
                                 isf15acp4::GraphicsBackend::Animator);
  g_frames = 0;
  isf15acp4::Animator anim(gfx, &CountFrame, nullptr, 25);  // 40 ms per frame
  CHECK(!anim.Tick(39));
  CHECK(anim.Tick(1));
  CHECK(anim.Tick(40));
  CHECK_EQ(g_frames, 2);
  CHECK_EQ(anim.Frame(), 2u);
  anim.Reset();
  CHECK_EQ(anim.Frame(), 0u);
}
