#ifndef ISF15ACP4_HEADER_INCLUDED
#error "isf15acp4.ipp must be included from isf15acp4.hpp"
#endif

namespace isf15acp4 {
namespace detail {

template <typename SpiType>
bool presenterDrawPixel(void* ctx, uint8_t x, uint8_t y, Color565 color) noexcept {
    return static_cast<SmartDisplay<SpiType>*>(ctx)->DrawPixel(x, y, color);
}

template <typename SpiType>
bool presenterDrawLine(void* ctx, Point a, Point b, Color565 color) noexcept {
    return static_cast<SmartDisplay<SpiType>*>(ctx)->DrawLine(a, b, color);
}

template <typename SpiType>
bool presenterDrawRect(void* ctx, Rect r, Color565 color, bool filled) noexcept {
    return static_cast<SmartDisplay<SpiType>*>(ctx)->DrawRectangle(r, color, filled);
}

template <typename SpiType>
bool presenterFillScreen(void* ctx, Color565 color) noexcept {
    return static_cast<SmartDisplay<SpiType>*>(ctx)->FillScreen(color);
}

template <typename SpiType>
bool presenterFlushCanvas(void* ctx, const Color565* data, std::size_t bytes) noexcept {
    return static_cast<SmartDisplay<SpiType>*>(ctx)->WriteColors(data, bytes / sizeof(Color565));
}

}  // namespace detail

template <typename SpiType>
SmartDisplay<SpiType>::SmartDisplay(SpiType* bus, const SmartDisplayConfig& config) noexcept
    : bus_(bus), config_(config), variant_(config.variant), geometry_(GetGeometry(config.variant)) {
    window_ = {geometry_.column_start, geometry_.row_start, geometry_.column_end, geometry_.row_end};
}

template <typename SpiType>
SmartDisplay<SpiType>::SmartDisplay(SpiType* bus, ProductVariant variant) noexcept
    : SmartDisplay(bus, SmartDisplayConfig{.variant = variant}) {}

template <typename SpiType>
void SmartDisplay<SpiType>::setError(Error e) noexcept {
    last_error_ = e;
    error_flags_ |= static_cast<uint16_t>(e);
}

template <typename SpiType>
void SmartDisplay<SpiType>::pulseReset() noexcept {
    // SSD1331 rev 1.2, section 7.9 and Table 18: hold RES# low (Reset asserted = Active) for at least 3 us, release
    // it high (Inactive), then allow the reset completion time (2 us max) before the first command.
    bus_->GpioSetActive(CtrlPin::Reset);
    bus_->DelayUs(ssd1331::kResetPulseMinUs);
    bus_->GpioSetInactive(CtrlPin::Reset);
    bus_->DelayUs(ssd1331::kResetRecoveryUs);
}

template <typename SpiType>
bool SmartDisplay<SpiType>::sendCommand(uint8_t cmd, const uint8_t* data, uint8_t len) noexcept {
    if (!bus_->BeginCommand()) {
        setError(Error::SpiWrite);
        return false;
    }

    if (!bus_->WriteBytes(&cmd, 1)) {
        bus_->EndTransaction();
        setError(Error::SpiWrite);
        return false;
    }

    if (len > 0 && data != nullptr) {
        if (!bus_->WriteBytes(data, len)) {
            bus_->EndTransaction();
            setError(Error::SpiWrite);
            return false;
        }
    }

    bus_->EndTransaction();
    return true;
}

template <typename SpiType>
bool SmartDisplay<SpiType>::sendCommandByte(uint8_t cmd, uint8_t data) noexcept {
    return sendCommand(cmd, &data, 1);
}

template <typename SpiType>
bool SmartDisplay<SpiType>::sendCommandOnly(uint8_t cmd) noexcept {
    return sendCommand(cmd, nullptr, 0);
}

template <typename SpiType>
bool SmartDisplay<SpiType>::runInitTable() noexcept {
    std::size_t count = 0;
    const ssd1331::InitStep* table = ssd1331::GetInitTable(variant_, count);

    for (std::size_t i = 0; i < count; ++i) {
        if (!sendCommand(table[i].command, table[i].data, table[i].data_len)) {
            return false;
        }
    }
    return true;
}

template <typename SpiType>
bool SmartDisplay<SpiType>::Initialize(bool enable_vcc) noexcept {
    if (bus_ == nullptr) {
        setError(Error::InvalidParam);
        return false;
    }

    if (!bus_->EnsureInitialized()) {
        setError(Error::NotInitialized);
        return false;
    }

    bus_->GpioSetInactive(CtrlPin::ChipSelect);
    bus_->GpioSetActive(CtrlPin::DataCommand);
    bus_->GpioSetInactive(CtrlPin::VccEnable);

    pulseReset();

    if (enable_vcc) {
        bus_->GpioSetActive(CtrlPin::VccEnable);
        bus_->DelayUs(1000);
    }

    if (!runInitTable()) {
        return false;
    }

    if (!SetFullWindow()) {
        return false;
    }

    if (config_.graphics == GraphicsBackend::HardwareDirect) {
        GraphicsContext gfx = CreateGraphicsContext();
        ConfigurePresenter(gfx);
    }

    initialized_ = true;
    return true;
}

template <typename SpiType>
bool SmartDisplay<SpiType>::Reset(bool enable_vcc) noexcept {
    initialized_ = false;
    return Initialize(enable_vcc);
}

template <typename SpiType>
bool SmartDisplay<SpiType>::DisplayOn() noexcept {
    return sendCommandOnly(static_cast<uint8_t>(ssd1331::Command::DisplayOn));
}

template <typename SpiType>
bool SmartDisplay<SpiType>::DisplayOff() noexcept {
    return sendCommandOnly(static_cast<uint8_t>(ssd1331::Command::DisplayOff));
}

template <typename SpiType>
bool SmartDisplay<SpiType>::DisplayDim() noexcept {
    return sendCommandOnly(0xAC);
}

template <typename SpiType>
bool SmartDisplay<SpiType>::SetMasterCurrent(uint8_t level) noexcept {
    if (level > 0x0F) {
        setError(Error::OutOfRange);
        return false;
    }
    return sendCommandByte(static_cast<uint8_t>(ssd1331::Command::MasterCurrent), level);
}

template <typename SpiType>
bool SmartDisplay<SpiType>::SetWindow(const Rect& rect) noexcept {
    if (rect.x1 < rect.x0 || rect.y1 < rect.y0 || rect.x1 >= geometry_.width ||
        rect.y1 >= geometry_.height) {
        setError(Error::OutOfRange);
        return false;
    }

    const uint8_t col_data[2] = {rect.x0, rect.x1};
    const uint8_t row_data[2] = {rect.y0, rect.y1};

    if (!sendCommand(static_cast<uint8_t>(ssd1331::Command::SetColumnAddress), col_data, 2)) {
        return false;
    }
    if (!sendCommand(static_cast<uint8_t>(ssd1331::Command::SetRowAddress), row_data, 2)) {
        return false;
    }

    window_ = rect;
    return true;
}

template <typename SpiType>
bool SmartDisplay<SpiType>::SetFullWindow() noexcept {
    return SetWindow({geometry_.column_start, geometry_.row_start, geometry_.column_end,
                      geometry_.row_end});
}

template <typename SpiType>
bool SmartDisplay<SpiType>::WritePixels(const uint8_t* data, std::size_t byte_len) noexcept {
    if (!initialized_) {
        setError(Error::NotInitialized);
        return false;
    }
    if (data == nullptr || byte_len == 0 || (byte_len % 2) != 0) {
        setError(Error::InvalidParam);
        return false;
    }

    if (!bus_->BeginData()) {
        setError(Error::SpiWrite);
        return false;
    }

    if (!bus_->WriteBytes(data, byte_len)) {
        bus_->EndTransaction();
        setError(Error::SpiWrite);
        return false;
    }

    bus_->EndTransaction();
    return true;
}

template <typename SpiType>
bool SmartDisplay<SpiType>::WriteColors(const Color565* colors, std::size_t count) noexcept {
    if (!initialized_) {
        setError(Error::NotInitialized);
        return false;
    }
    if (colors == nullptr || count == 0) {
        setError(Error::InvalidParam);
        return false;
    }

    if (!bus_->BeginData()) {
        setError(Error::SpiWrite);
        return false;
    }

    // SSD1331 rev 1.2 section 7.5.2, Table 9 (65k colour, 8-bit interface): first byte C4..C0 B5..B3, second byte
    // B2..B0 A4..A0, i.e. the 16-bit pixel high byte first. Serialise explicitly so the CPU's byte order does not
    // matter, in chunks inside one data transaction.
    constexpr std::size_t kChunkPixels = 64;
    uint8_t chunk[kChunkPixels * 2];
    std::size_t done = 0;
    while (done < count) {
        const std::size_t n = (count - done) < kChunkPixels ? (count - done) : kChunkPixels;
        for (std::size_t i = 0; i < n; ++i) {
            const Color565 c = colors[done + i];
            chunk[2 * i] = static_cast<uint8_t>(c >> 8);
            chunk[2 * i + 1] = static_cast<uint8_t>(c & 0xFF);
        }
        if (!bus_->WriteBytes(chunk, n * 2)) {
            bus_->EndTransaction();
            setError(Error::SpiWrite);
            return false;
        }
        done += n;
    }

    bus_->EndTransaction();
    return true;
}

template <typename SpiType>
bool SmartDisplay<SpiType>::DrawCanvas(const Canvas& canvas) noexcept {
    if (canvas.Width() != geometry_.width || canvas.Height() != geometry_.height) {
        setError(Error::InvalidParam);
        return false;
    }
    if (!SetFullWindow()) {
        return false;
    }
    return WriteColors(canvas.Data(), canvas.ByteSize() / sizeof(Color565));
}

template <typename SpiType>
GraphicsContext SmartDisplay<SpiType>::CreateGraphicsContext() const noexcept {
    return GraphicsContext(variant_, config_.graphics);
}

template <typename SpiType>
void SmartDisplay<SpiType>::ConfigurePresenter(GraphicsContext& gfx) noexcept {
    DisplayPresenter presenter{};
    presenter.context = this;
    presenter.draw_pixel = &detail::presenterDrawPixel<SpiType>;
    presenter.draw_line = &detail::presenterDrawLine<SpiType>;
    presenter.draw_rect = &detail::presenterDrawRect<SpiType>;
    presenter.fill_screen = &detail::presenterFillScreen<SpiType>;
    presenter.flush_canvas = &detail::presenterFlushCanvas<SpiType>;
    gfx.BindPresenter(presenter);
}

template <typename SpiType>
bool SmartDisplay<SpiType>::Present(const GraphicsContext& gfx) noexcept {
    if (gfx.Backend() == GraphicsBackend::HardwareDirect) {
        return true;
    }
    return DrawCanvas(gfx.CanvasBuffer());
}

template <typename SpiType>
bool SmartDisplay<SpiType>::DrawPixel(uint8_t x, uint8_t y, Color565 color) noexcept {
    if (!SetWindow({x, y, x, y})) {
        return false;
    }
    const uint8_t bytes[2] = {static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color & 0xFF)};
    return WritePixels(bytes, sizeof(bytes));
}

template <typename SpiType>
bool SmartDisplay<SpiType>::FillScreen(Color565 color) noexcept {
    Canvas canvas(variant_);
    canvas.Clear(color);
    return DrawCanvas(canvas);
}

template <typename SpiType>
void SmartDisplay<SpiType>::colorToSsd1331(Color565 color, uint8_t out[3]) noexcept {
    out[0] = static_cast<uint8_t>((color >> 11) & 0x1F);  // A: red 5-bit
    out[1] = static_cast<uint8_t>((color >> 5) & 0x3F);   // B: green 6-bit
    out[2] = static_cast<uint8_t>(color & 0x1F);          // C: blue 5-bit
}

template <typename SpiType>
bool SmartDisplay<SpiType>::DrawLine(const Point& start, const Point& end, Color565 color) noexcept {
    uint8_t rgb[3];
    colorToSsd1331(color, rgb);
    const uint8_t data[7] = {start.x, start.y, end.x, end.y,
                             static_cast<uint8_t>(rgb[2] << 1), rgb[1],
                             static_cast<uint8_t>(rgb[0] << 1)};
    return sendCommand(static_cast<uint8_t>(ssd1331::Command::DrawLine), data, sizeof(data));
}

template <typename SpiType>
bool SmartDisplay<SpiType>::DrawRectangle(const Rect& rect, Color565 color, bool filled) noexcept {
    if (filled) {
        const uint8_t fill_cfg[2] = {0x01, 0x00};
        if (!sendCommand(static_cast<uint8_t>(ssd1331::Command::FillEnable), fill_cfg, 2)) {
            return false;
        }
    }

    uint8_t rgb[3];
    colorToSsd1331(color, rgb);
    const uint8_t data[10] = {rect.x0, rect.y0, rect.x1, rect.y1,
                              static_cast<uint8_t>(rgb[2] << 1), rgb[1],
                              static_cast<uint8_t>(rgb[0] << 1),
                              static_cast<uint8_t>(rgb[2] << 1), rgb[1],
                              static_cast<uint8_t>(rgb[0] << 1)};
    const bool ok = sendCommand(static_cast<uint8_t>(ssd1331::Command::DrawRectangle), data,
                                sizeof(data));

    if (filled) {
        const uint8_t disable[2] = {0x00, 0x00};
        sendCommand(static_cast<uint8_t>(ssd1331::Command::FillEnable), disable, 2);
    }
    return ok;
}

template <typename SpiType>
bool SmartDisplay<SpiType>::ClearWindow(const Rect& rect) noexcept {
    const uint8_t data[4] = {rect.x0, rect.y0, rect.x1, rect.y1};
    return sendCommand(static_cast<uint8_t>(ssd1331::Command::ClearWindow), data, sizeof(data));
}

template <typename SpiType>
bool SmartDisplay<SpiType>::IsPressed() const noexcept {
    return bus_ != nullptr && bus_->GpioRead(CtrlPin::Switch);
}

template <typename SpiType>
bool SmartDisplay<SpiType>::ConfigureScroll(uint8_t h_offset, uint8_t start_row, uint8_t scroll_rows,
                                            uint8_t v_offset, uint8_t interval_code,
                                            bool activate) noexcept {
    const uint8_t data[5] = {h_offset, start_row, scroll_rows, v_offset,
                              static_cast<uint8_t>(interval_code & 0x03)};
    if (!sendCommand(static_cast<uint8_t>(ssd1331::Command::ScrollSetup), data, sizeof(data))) {
        return false;
    }
    return sendCommandOnly(activate ? static_cast<uint8_t>(ssd1331::Command::ActivateScroll)
                                    : static_cast<uint8_t>(ssd1331::Command::DeactivateScroll));
}

}  // namespace isf15acp4
