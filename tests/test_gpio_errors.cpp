/**
 * @file test_gpio_errors.cpp
 * @brief A control pin the port could not drive fails Initialize() and is reported:
 * Error::GpioControl for SS and D/C, Error::GpioControl plus Error::PowerSequence for RES# and the
 * VCC enable (SSD1331 rev 1.2, section 7.9 and Table 18: the reset pulse and VCC come before the
 * first command).
 */
#include "isf15acp4.hpp"
#include "mock_spi.hpp"
#include "test_harness.hpp"

#include <type_traits>
#include <utility>

using isf15acp4::CtrlPin;
using isf15acp4::Error;
using Display = isf15acp4::SmartDisplay<isf15acp4_test::MockSpi>;

// The port contract: GpioSet() returns whether the pin was driven.
static_assert(std::is_same_v<decltype(std::declval<isf15acp4::SpiInterface<isf15acp4_test::MockSpi>&>()
                                          .GpioSet(CtrlPin::Reset, isf15acp4::GpioSignal::Active)),
                             bool>,
              "SpiInterface::GpioSet() must return bool");

namespace {
void checkFailsWith(CtrlPin pin, bool power_sequence) {
  isf15acp4_test::MockSpi spi;
  spi.gpio_fails = true;
  spi.failing_pin = pin;
  Display display(&spi);
  CHECK(!display.Initialize(true));
  CHECK(!display.IsInitialized());
  CHECK(display.HasError(Error::GpioControl));
  CHECK(display.HasError(Error::PowerSequence) == power_sequence);
  CHECK(display.GetLastError() == (power_sequence ? Error::PowerSequence : Error::GpioControl));
  CHECK(spi.transactions.empty()); // no command after a failed pin
}
} // namespace

TEST_CASE("Initialize fails with GpioControl when SS cannot be driven") {
  checkFailsWith(CtrlPin::ChipSelect, false);
}

TEST_CASE("Initialize fails with GpioControl when D/C cannot be driven") {
  checkFailsWith(CtrlPin::DataCommand, false);
}

TEST_CASE("Initialize fails with PowerSequence when RES# cannot be driven") {
  checkFailsWith(CtrlPin::Reset, true);
}

TEST_CASE("Initialize fails with PowerSequence when VCC cannot be switched") {
  checkFailsWith(CtrlPin::VccEnable, true);
}

TEST_CASE("Initialize(false) with working pins reports no error") {
  isf15acp4_test::MockSpi spi;
  Display display(&spi);
  CHECK(display.Initialize(false));
  CHECK_EQ(display.GetErrorFlags(), 0u);
}
