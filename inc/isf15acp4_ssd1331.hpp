/**
 * @file isf15acp4_ssd1331.hpp
 * @brief SSD1331 command constants and initialization tables for NKK SmartDisplay.
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "isf15acp4_types.hpp"

namespace isf15acp4::ssd1331 {

/** @brief SSD1331 command opcodes used by NKK application notes. */
enum class Command : uint8_t {
    SetColumnAddress = 0x15,
    SetRowAddress = 0x75,
    SetContrastA = 0x81,
    SetContrastB = 0x82,
    SetContrastC = 0x83,
    SecondPrechargeA = 0x8A,
    SecondPrechargeB = 0x8B,
    SecondPrechargeC = 0x8C,
    MasterCurrent = 0x87,
    RemapColorDepth = 0xA0,
    SetDisplayStartLine = 0xA1,
    SetDisplayOffset = 0xA2,
    NormalDisplay = 0xA4,
    AllPixelsOnMax = 0xA5,
    AllPixelsOnMin = 0xA6,
    InverseDisplay = 0xA7,
    SetMultiplexRatio = 0xA8,
    DimModeSetting = 0xAB,
    MasterConfig = 0xAD,
    DisplayOff = 0xAE,
    DisplayOn = 0xAF,
    PowerSave = 0xB0,
    PhasePeriod = 0xB1,
    DisplayClockDiv = 0xB3,
    LinearGrayScale = 0xB9,
    PrechargeLevel = 0xBB,
    Vcomh = 0xBE,
    DrawLine = 0x21,
    DimWindow = 0x24,
    ClearWindow = 0x25,
    DrawRectangle = 0x22,
    Copy = 0x23,
    FillEnable = 0x26,
    ScrollSetup = 0x27,
    DeactivateScroll = 0x2E,
    ActivateScroll = 0x2F,
    NopBd = 0xBD,
    NopBc = 0xBC,
    NopE3 = 0xE3
};

/** @brief One initialization command with optional trailing data bytes. */
struct InitStep {
    uint8_t command;
    uint8_t data[10];
    uint8_t data_len;
};

/** @brief Minimum RES# low pulse width (SSD1331 rev 1.2 Table 18; NKK power ON sequence). */
inline constexpr uint32_t kResetPulseMinUs = 3;

/**
 * @brief Wait after releasing RES# before the first command.
 *
 * The SSD1331 reset completion time is 2 us maximum (rev 1.2 Table 18); the driver waits longer for margin.
 */
inline constexpr uint32_t kResetRecoveryUs = 10;

/** @brief Maximum SPI clock per datasheet (6.6 MHz). */
inline constexpr uint32_t kMaxSpiClockHz = 6'600'000;

/**
 * @brief Factory initialization table for ISF15ACP4 (96x64 frameless).
 * @return Pointer to static initialization sequence (Table 3, NKK app notes).
 */
inline const InitStep* GetInitTableIsf15acp4(std::size_t& count) noexcept {
    static constexpr InitStep kTable[] = {
        {0x81, {0x1C}, 1},
        {0x82, {0x2E}, 1},
        {0x83, {0x2C}, 1},
        {0x8A, {0x4F}, 1},
        {0x8B, {0x74}, 1},
        {0x8C, {0x88}, 1},
        {0x87, {0x0F}, 1},
        {0xA0, {0x68}, 1},
        {0xA1, {0x00}, 1},
        {0xA2, {0x00}, 1},
        {0xA4, {}, 0},
        {0xA8, {0x3F}, 1},
        {0xAB, {0x00, 0x0E, 0x17, 0x16, 0x12}, 5},
        {0xAD, {0x8E}, 1},
        {0xB0, {0x0B}, 1},
        {0xB1, {0x17}, 1},
        {0xB3, {0xF0}, 1},
        {0xB9, {}, 0},
        {0xBB, {0x1D}, 1},
        {0xBE, {0x20}, 1},
        {0xAF, {}, 0},
    };
    count = sizeof(kTable) / sizeof(kTable[0]);
    return kTable;
}

/**
 * @brief Initialization table for ISC15ANP4 (64x48 pushbutton).
 */
inline const InitStep* GetInitTableIsc15anp4(std::size_t& count) noexcept {
    static constexpr InitStep kTable[] = {
        {0x81, {0x15}, 1}, {0x82, {0x1A}, 1}, {0x83, {0x17}, 1}, {0x87, {0x0F}, 1},
        {0xA0, {0x70}, 1}, {0xA1, {0x00}, 1}, {0xA2, {0x10}, 1}, {0xA4, {}, 0},
        {0xA8, {0x2F}, 1}, {0xAB, {0x00, 0x12, 0x0C, 0x14, 0x12}, 5}, {0xAD, {0x8E}, 1},
        {0xB0, {0x0B}, 1}, {0xB1, {0x44}, 1}, {0xB3, {0xA0}, 1}, {0xB9, {}, 0},
        {0xBB, {0x12}, 1}, {0xBE, {0x3E}, 1}, {0xAF, {}, 0},
    };
    count = sizeof(kTable) / sizeof(kTable[0]);
    return kTable;
}

/**
 * @brief Initialization table for ISC01P (52x36 display).
 */
inline const InitStep* GetInitTableIsc01p(std::size_t& count) noexcept {
    static constexpr InitStep kTable[] = {
        {0x81, {0x0E}, 1}, {0x82, {0x13}, 1}, {0x83, {0x12}, 1}, {0x87, {0x0F}, 1},
        {0xA0, {0x70}, 1}, {0xA1, {0x00}, 1}, {0xA2, {0x1C}, 1}, {0xA4, {}, 0},
        {0xA8, {0x23}, 1}, {0xAB, {0x00, 0x07, 0x0A, 0x09, 0x12}, 5}, {0xAD, {0x8E}, 1},
        {0xB0, {0x0B}, 1}, {0xB1, {0x44}, 1}, {0xB3, {0x30}, 1}, {0xB9, {}, 0},
        {0xBB, {0x12}, 1}, {0xBE, {0x3C}, 1}, {0xAF, {}, 0},
    };
    count = sizeof(kTable) / sizeof(kTable[0]);
    return kTable;
}

/**
 * @brief Resolve initialization table for a product variant.
 */
inline const InitStep* GetInitTable(ProductVariant variant, std::size_t& count) noexcept {
    switch (variant) {
        case ProductVariant::Isf15acp4:
            return GetInitTableIsf15acp4(count);
        case ProductVariant::Isc15anp4:
            return GetInitTableIsc15anp4(count);
        case ProductVariant::Isc01p:
            return GetInitTableIsc01p(count);
    }
    return GetInitTableIsf15acp4(count);
}

}  // namespace isf15acp4::ssd1331
