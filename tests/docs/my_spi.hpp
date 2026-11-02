/**
 * @file my_spi.hpp
 * @brief The SPI port shown on the documentation's Quickstart and Porting pages, compiled by the host build.
 *
 * Every platform call goes through the `platform_*` functions declared below; a real port replaces them with its
 * SDK (ESP-IDF, STM32 HAL, Zephyr, Linux spidev). The class is what the driver needs, nothing more.
 */
#pragma once

#include "isf15acp4.hpp"

#include <cstddef>
#include <cstdint>

// Platform hooks: your SDK's calls. Declared only, so the docs build checks the port compiles.
bool platform_spi_init() noexcept;                                       // SPI master, mode 0, <= 6.6 MHz
bool platform_spi_write(const uint8_t* data, std::size_t len) noexcept;  // blocking: returns when sent
void platform_gpio_write(int gpio, bool level) noexcept;
bool platform_gpio_read(int gpio) noexcept;
void platform_delay_us(uint32_t us) noexcept;

// --8<-- [start:port]
class MySpi : public isf15acp4::SpiInterface<MySpi> {
public:
  // Your board's GPIO numbers.
  static constexpr int kCs = 10, kDc = 13, kRes = 14, kVccEn = 9, kSwitch = 8;

  bool EnsureInitialized() noexcept {
    platform_gpio_write(kCs, true);       // SS idle high
    platform_gpio_write(kVccEn, false);   // 15 V drive supply off until after reset
    return platform_spi_init();
  }

  // D/C low = command, high = data; SS low for the whole transaction.
  bool BeginCommand() noexcept { return begin(false); }
  bool BeginData() noexcept { return begin(true); }
  void EndTransaction() noexcept { platform_gpio_write(kCs, true); }

  // Must not return before the last bit is on the wire: SS rises right after.
  bool WriteBytes(const uint8_t* data, std::size_t len) noexcept {
    return platform_spi_write(data, len);
  }

  void GpioSet(isf15acp4::CtrlPin pin, isf15acp4::GpioSignal signal) noexcept {
    const bool active = signal == isf15acp4::GpioSignal::Active;
    switch (pin) {
      case isf15acp4::CtrlPin::ChipSelect:  platform_gpio_write(kCs, !active); break;   // SS active low
      case isf15acp4::CtrlPin::DataCommand: platform_gpio_write(kDc, active); break;    // high = data
      case isf15acp4::CtrlPin::Reset:       platform_gpio_write(kRes, !active); break;  // RES# active low
      case isf15acp4::CtrlPin::VccEnable:   platform_gpio_write(kVccEn, active); break;
      case isf15acp4::CtrlPin::Switch:      break;                                      // input only
    }
  }

  // The switch closes pins 1-2; wired to GND with a pull-up, pressed reads low.
  bool GpioRead(isf15acp4::CtrlPin pin) noexcept {
    return pin == isf15acp4::CtrlPin::Switch && !platform_gpio_read(kSwitch);
  }

  void DelayUs(uint32_t us) noexcept { platform_delay_us(us); }

private:
  bool begin(bool data) noexcept {
    platform_gpio_write(kDc, data);
    platform_gpio_write(kCs, false);
    return true;
  }
};
// --8<-- [end:port]
