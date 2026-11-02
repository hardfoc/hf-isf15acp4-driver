/**
 * @file isf15acp4.hpp
 * @brief Hardware-agnostic driver for NKK SmartDisplay OLED pushbuttons and displays.
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "isf15acp4_button_ui.hpp"
#include "isf15acp4_canvas.hpp"
#include "isf15acp4_graphics_context.hpp"
#include "isf15acp4_spi_interface.hpp"
#include "isf15acp4_ssd1331.hpp"
#include "isf15acp4_types.hpp"
#include "isf15acp4_version.h"

namespace isf15acp4 {

/**
 * @class SmartDisplay
 * @brief High-level driver for NKK SSD1331-based SmartDisplay modules.
 *
 * Supports ISF15ACP4 (96x64 frameless pushbutton), ISC15ANP4 (64x48), and
 * ISC01P (52x36 display). Communication uses SPI with separate D/C and SS lines.
 *
 * @tparam SpiType Platform adapter implementing SpiInterface CRTP contract.
 */
template <typename SpiType>
class SmartDisplay {
public:
    /**
     * @brief Construct with explicit configuration (variant + graphics backend).
     */
    SmartDisplay(SpiType* bus, const SmartDisplayConfig& config) noexcept;

    /**
     * @brief Construct a SmartDisplay driver (defaults to ISF15ACP4 + BuiltinCanvas).
     * @param bus Pointer to platform SPI/GPIO adapter.
     * @param variant Product variant initialization table selector.
     */
    SmartDisplay(SpiType* bus, ProductVariant variant = ProductVariant::Isf15acp4) noexcept;

    /**
     * @brief Perform power-up, reset, and SSD1331 initialization.
     *
     * Sequence:
     * 1. Ensure SPI/GPIO ready
     * 2. Assert Reset (RES# low) >= 3 us, release it (high), wait for reset completion
     * 3. Optionally enable VCC rail via VccEnable pin
     * 4. Transmit variant-specific initialization table
     * 5. Configure default memory window
     *
     * @param enable_vcc When true, asserts VccEnable after reset.
     * @return true on success.
     */
    bool Initialize(bool enable_vcc = true) noexcept;

    /** @brief Returns true after successful Initialize(). */
    [[nodiscard]] bool IsInitialized() const noexcept { return initialized_; }

    /** @brief Hardware reset pulse and full re-initialization. */
    bool Reset(bool enable_vcc = true) noexcept;

    /** @brief Turn display on (normal mode, command AFH). */
    bool DisplayOn() noexcept;

    /** @brief Turn display off / sleep (command AEH). */
    bool DisplayOff() noexcept;

    /** @brief Enter dim mode (command ACH). */
    bool DisplayDim() noexcept;

    /**
     * @brief Set master current while display is off (command 87H).
     * @param level 0x00 (min) to 0x0F (max brightness).
     */
    bool SetMasterCurrent(uint8_t level) noexcept;

    /**
     * @brief Configure the active GRAM download window.
     * @param rect Inclusive pixel rectangle in display coordinates (0..width-1, 0..height-1). The driver adds the
     *             variant's RAM column/row offset (DisplayGeometry::column_start / row_start).
     */
    bool SetWindow(const Rect& rect) noexcept;

    /** @brief Configure full-panel memory window for the active variant. */
    bool SetFullWindow() noexcept;

    /**
     * @brief Stream raw pixel bytes to the controller GRAM, unchanged.
     *
     * The bytes must already be in the panel's 65k-colour format: two bytes per pixel, high byte first, blue in
     * bits 15..11 and red in bits 4..0 (see ssd1331::ToPanelPixel()). For Color565 buffers use WriteColors().
     *
     * @param data Pixel buffer (two bytes per pixel).
     * @param byte_len Number of bytes (must be even).
     */
    bool WritePixels(const uint8_t* data, std::size_t byte_len) noexcept;

    /**
     * @brief Stream Color565 pixels to the controller GRAM in the SSD1331 65k-colour byte order.
     *
     * Each pixel is converted to the panel's colour order (ssd1331::ToPanelPixel(): blue in the high bits) and goes
     * out high byte first (SSD1331 rev 1.2 section 7.5.2, Table 9), whatever the CPU's byte order, in one data
     * transaction. Use this instead of WritePixels() for Color565 buffers.
     *
     * @param colors Pixel buffer.
     * @param count Number of pixels (not bytes).
     */
    bool WriteColors(const Color565* colors, std::size_t count) noexcept;

    /** @brief Upload an entire canvas framebuffer. */
    bool DrawCanvas(const Canvas& canvas) noexcept;

    /** @brief Present any graphics backend to the panel. */
    bool Present(const GraphicsContext& gfx) noexcept;

    /**
     * @brief Create a graphics context matching this display configuration.
     *
     * With GraphicsBackend::HardwareDirect the returned context is already bound to this display
     * (ConfigurePresenter()), so its shapes go to the panel at once. The display must outlive the context.
     */
    [[nodiscard]] GraphicsContext CreateGraphicsContext() noexcept;

    /**
     * @brief Create an unbound graphics context matching this display configuration.
     *
     * A const display cannot bind itself: for HardwareDirect, call ConfigurePresenter() on the result.
     */
    [[nodiscard]] GraphicsContext CreateGraphicsContext() const noexcept;

    /** @brief Register presenter hooks for HardwareDirect backend (bind this display to @p gfx). */
    void ConfigurePresenter(GraphicsContext& gfx) noexcept;

    /** @brief Active graphics backend selected at construction. */
    [[nodiscard]] GraphicsBackend GetGraphicsBackend() const noexcept { return config_.graphics; }

    /** @brief Active driver configuration. */
    [[nodiscard]] const SmartDisplayConfig& GetConfig() const noexcept { return config_; }

    /** @brief Draw one RGB565 pixel at (x,y). */
    bool DrawPixel(uint8_t x, uint8_t y, Color565 color) noexcept;

    /** @brief Fill the panel with a solid color. */
    bool FillScreen(Color565 color) noexcept;

    /** @brief Draw a hardware-accelerated line (command 21H), in display coordinates. */
    bool DrawLine(const Point& start, const Point& end, Color565 color) noexcept;

    /** @brief Draw a hardware rectangle outline (command 22H), in display coordinates. */
    bool DrawRectangle(const Rect& rect, Color565 color, bool filled = false) noexcept;

    /** @brief Clear a rectangular window via controller command 25H, in display coordinates. */
    bool ClearWindow(const Rect& rect) noexcept;

    /**
     * @brief Read momentary switch state when wired to SpiInterface::CtrlPin::Switch.
     * @return true when the switch is pressed (active).
     */
    [[nodiscard]] bool IsPressed() const noexcept;

    /**
     * @brief Set up hardware scrolling and optionally start it.
     *
     * Always stops scrolling first (2EH), then sends the setup (27H) and, when @p activate is true, starts it
     * (2FH): the SSD1331 must not get new scroll parameters while scrolling runs (rev 1.2 Table 10, 27H note 2).
     * With @p activate false the scroll stays stopped; rewrite the display RAM afterwards (see StopScroll()).
     */
    bool ConfigureScroll(uint8_t h_offset, uint8_t start_row, uint8_t scroll_rows,
                         uint8_t v_offset, uint8_t interval_code, bool activate) noexcept;

    /**
     * @brief Stop hardware scrolling (2EH).
     *
     * The display RAM stays shifted: the SSD1331 needs it rewritten after 2EH (rev 1.2 Table 10). Prefer
     * StopScroll(const GraphicsContext&), or redraw yourself.
     */
    bool StopScroll() noexcept;

    /** @brief Stop hardware scrolling (2EH), then rewrite the display RAM with Present(@p gfx). */
    bool StopScroll(const GraphicsContext& gfx) noexcept;

    [[nodiscard]] uint16_t GetErrorFlags() const noexcept { return error_flags_; }
    [[nodiscard]] bool HasError(Error e) const noexcept {
        return (error_flags_ & static_cast<uint16_t>(e)) != 0;
    }
    void ClearErrorFlags(uint16_t mask = 0xFFFF) noexcept { error_flags_ &= static_cast<uint16_t>(~mask); }
    [[nodiscard]] Error GetLastError() const noexcept { return last_error_; }

    [[nodiscard]] const DisplayGeometry& Geometry() const noexcept { return geometry_; }
    [[nodiscard]] ProductVariant Variant() const noexcept { return variant_; }

    static constexpr const char* GetDriverVersion() noexcept { return HF_ISF15ACP4_VERSION_STRING; }
    static constexpr uint8_t GetDriverVersionMajor() noexcept { return HF_ISF15ACP4_VERSION_MAJOR; }
    static constexpr uint8_t GetDriverVersionMinor() noexcept { return HF_ISF15ACP4_VERSION_MINOR; }
    static constexpr uint8_t GetDriverVersionPatch() noexcept { return HF_ISF15ACP4_VERSION_PATCH; }

private:
    SpiType* bus_;
    SmartDisplayConfig config_;
    ProductVariant variant_;
    DisplayGeometry geometry_;
    Rect window_{};
    bool initialized_{false};
    Error last_error_{Error::None};
    uint16_t error_flags_{0};

    void setError(Error e) noexcept;
    bool sendCommand(uint8_t cmd, const uint8_t* data, uint8_t len) noexcept;
    bool sendCommandByte(uint8_t cmd, uint8_t data) noexcept;
    bool sendCommandOnly(uint8_t cmd) noexcept;
    void pulseReset() noexcept;
    bool runInitTable() noexcept;
    static void colorToSsd1331(Color565 color, uint8_t out[3]) noexcept;
    [[nodiscard]] Rect toRam(const Rect& rect) const noexcept;
};

inline const char* GetDriverVersion() noexcept { return HF_ISF15ACP4_VERSION_STRING; }

}  // namespace isf15acp4

#define ISF15ACP4_HEADER_INCLUDED
#include "../src/isf15acp4.ipp"
#undef ISF15ACP4_HEADER_INCLUDED
