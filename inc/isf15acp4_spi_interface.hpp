/**
 * @file isf15acp4_spi_interface.hpp
 * @brief CRTP-based SPI and GPIO interface for NKK SmartDisplay OLED drivers.
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace isf15acp4 {

/**
 * @enum CtrlPin
 * @brief Hardware control signals for the SSD1331 SmartDisplay interface.
 *
 * The driver uses abstract ACTIVE/INACTIVE levels so platform adapters can map
 * polarity correctly (e.g. CS is active-low on the physical pin).
 */
enum class CtrlPin : uint8_t {
    ChipSelect = 0,   ///< SS / slave select (active-low on device)
    DataCommand = 1,  ///< D/C: command (inactive) vs data (active)
    Reset = 2,        ///< RES# reset input: Active = held in reset (pin low), Inactive = running (pin high)
    VccEnable = 3,    ///< Optional charge-pump / 16V rail enable
    Switch = 4        ///< Optional momentary switch sense input
};

/**
 * @enum GpioSignal
 * @brief Abstract GPIO level for control pins.
 */
enum class GpioSignal : uint8_t {
    Inactive = 0,
    Active = 1
};

/**
 * @enum SpiMode
 * @brief SPI mode required by NKK SmartDisplay devices.
 *
 * Data is sampled on the rising edge of SCK (mode 0 or 3 depending on CPOL).
 * Engineering kits commonly use CPOL=0, CPHA=0.
 */
enum class SpiMode : uint8_t {
    Mode0 = 0,
    Mode3 = 3
};

/**
 * @brief CRTP transport interface for SPI writes and GPIO control.
 *
 * Platform implementations must:
 * - Hold SS low for the entire command+data or image stream
 * - Wait for SPI completion before deasserting SS
 * - Drive D/C low for commands and high for pixel data
 *
 * @tparam Derived Concrete platform adapter type.
 */
template <typename Derived>
class SpiInterface {
public:
    /**
     * @brief Ensure SPI bus and GPIO pins are configured.
     * @return true when the bus is ready.
     */
    bool EnsureInitialized() noexcept {
        return static_cast<Derived*>(this)->EnsureInitialized();
    }

    /**
     * @brief Begin an SPI transaction with D/C in command mode.
     * @return true on success.
     */
    bool BeginCommand() noexcept { return static_cast<Derived*>(this)->BeginCommand(); }

    /**
     * @brief Begin an SPI transaction with D/C in data mode.
     * @return true on success.
     */
    bool BeginData() noexcept { return static_cast<Derived*>(this)->BeginData(); }

    /**
     * @brief End the active SPI transaction (typically deassert CS).
     */
    void EndTransaction() noexcept { static_cast<Derived*>(this)->EndTransaction(); }

    /**
     * @brief Write bytes on MOSI while a transaction is active.
     * @param data Source buffer.
     * @param len Number of bytes.
     * @return true on success.
     */
    bool WriteBytes(const uint8_t* data, std::size_t len) noexcept {
        return static_cast<Derived*>(this)->WriteBytes(data, len);
    }

    /**
     * @brief Drive a control pin to an abstract signal level.
     * @param pin Control signal identifier.
     * @param signal Desired abstract level.
     */
    void GpioSet(CtrlPin pin, GpioSignal signal) noexcept {
        static_cast<Derived*>(this)->GpioSet(pin, signal);
    }

    /**
     * @brief Read a digital input (typically the momentary switch).
     * @param pin Input pin identifier.
     * @return true when the abstract ACTIVE level is sensed.
     */
    bool GpioRead(CtrlPin pin) noexcept { return static_cast<Derived*>(this)->GpioRead(pin); }

    /**
     * @brief Blocking microsecond delay.
     * @param us Delay duration in microseconds.
     */
    void DelayUs(uint32_t us) noexcept { static_cast<Derived*>(this)->DelayUs(us); }

    void GpioSetActive(CtrlPin pin) noexcept { GpioSet(pin, GpioSignal::Active); }
    void GpioSetInactive(CtrlPin pin) noexcept { GpioSet(pin, GpioSignal::Inactive); }

    SpiInterface(const SpiInterface&) = delete;
    SpiInterface& operator=(const SpiInterface&) = delete;

protected:
    SpiInterface() = default;
    ~SpiInterface() = default;
};

}  // namespace isf15acp4
