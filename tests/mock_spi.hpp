/**
 * @file mock_spi.hpp
 * @brief A recording SpiInterface port for the host unit tests.
 *
 * Every transaction (D/C level, bytes written between Begin*() and EndTransaction()), every control-pin change and
 * every delay is recorded, so tests can check the exact SSD1331 byte stream and the reset / VCC sequence the driver
 * produces. Time is virtual: DelayUs() only advances a counter.
 */
#pragma once

#include "isf15acp4.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace isf15acp4_test {

/// One CS-low transaction: command (D/C low) or data (D/C high) and the bytes clocked out.
struct Transaction {
  bool data = false;
  std::vector<uint8_t> bytes;
};

/// One control-pin change, stamped with the virtual time in microseconds.
struct PinEvent {
  isf15acp4::CtrlPin pin;
  isf15acp4::GpioSignal signal;
  uint64_t time_us;
};

class MockSpi : public isf15acp4::SpiInterface<MockSpi> {
public:
  // Behaviour switches for fault injection.
  bool init_ok = true;
  bool write_ok = true;
  bool switch_pressed = false;

  // What the driver did.
  std::vector<Transaction> transactions;
  std::vector<PinEvent> pins;
  uint64_t now_us = 0;
  int open_transactions = 0;  // Begin*() without EndTransaction(): must stay 0 between calls

  bool EnsureInitialized() noexcept { return init_ok; }

  bool BeginCommand() noexcept { return begin(false); }
  bool BeginData() noexcept { return begin(true); }

  void EndTransaction() noexcept { --open_transactions; }

  bool WriteBytes(const uint8_t* data, std::size_t len) noexcept {
    if (!write_ok || open_transactions != 1 || transactions.empty()) {
      return false;
    }
    transactions.back().bytes.insert(transactions.back().bytes.end(), data, data + len);
    return true;
  }

  /// Fault injection: GpioSet() on this pin fails (and does not change it). Switch = none.
  isf15acp4::CtrlPin failing_pin = isf15acp4::CtrlPin::Switch;
  bool gpio_fails = false;

  bool GpioSet(isf15acp4::CtrlPin pin, isf15acp4::GpioSignal signal) noexcept {
    if (gpio_fails && pin == failing_pin) {
      return false;
    }
    pins.push_back({pin, signal, now_us});
    return true;
  }

  bool GpioRead(isf15acp4::CtrlPin pin) noexcept {
    return pin == isf15acp4::CtrlPin::Switch && switch_pressed;
  }

  void DelayUs(uint32_t us) noexcept { now_us += us; }

  /// Command transactions only, as {opcode, parameters...}.
  [[nodiscard]] std::vector<std::vector<uint8_t>> commands() const {
    std::vector<std::vector<uint8_t>> out;
    for (const auto& t : transactions) {
      if (!t.data) {
        out.push_back(t.bytes);
      }
    }
    return out;
  }

  void clear() {
    transactions.clear();
    pins.clear();
  }

private:
  bool begin(bool data) {
    ++open_transactions;
    transactions.push_back({data, {}});
    return true;
  }
};

}  // namespace isf15acp4_test
