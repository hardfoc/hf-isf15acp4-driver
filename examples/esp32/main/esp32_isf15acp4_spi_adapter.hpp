/**
 * @file esp32_isf15acp4_spi_adapter.hpp
 * @brief ESP-IDF SPI/GPIO adapter for the ISF15ACP4 SmartDisplay driver.
 */
#pragma once

#include <cstddef>
#include <cstdint>

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "isf15acp4_spi_interface.hpp"

namespace esp32_isf15acp4 {

struct PinConfig {
    gpio_num_t cs;
    gpio_num_t dc;
    gpio_num_t reset;
    gpio_num_t vcc_enable;
    gpio_num_t switch_in;
    bool switch_active_low;
};

struct SpiBusConfig {
    spi_host_device_t host;
    gpio_num_t mosi;
    gpio_num_t sclk;
    int clock_hz;
};

/**
 * @brief ESP-IDF implementation of isf15acp4::SpiInterface CRTP contract.
 */
class Esp32SpiAdapter : public isf15acp4::SpiInterface<Esp32SpiAdapter> {
public:
    Esp32SpiAdapter(const SpiBusConfig& bus, const PinConfig& pins) noexcept
        : bus_(bus), pins_(pins) {}

    bool EnsureInitialized() noexcept {
        if (ready_) {
            return true;
        }

        spi_bus_config_t bus_cfg = {};
        bus_cfg.mosi_io_num = bus_.mosi;
        bus_cfg.miso_io_num = GPIO_NUM_NC;
        bus_cfg.sclk_io_num = bus_.sclk;
        bus_cfg.quadwp_io_num = GPIO_NUM_NC;
        bus_cfg.quadhd_io_num = GPIO_NUM_NC;
        bus_cfg.max_transfer_sz = 96 * 64 * 2;

        const esp_err_t bus_err = spi_bus_initialize(bus_.host, &bus_cfg, SPI_DMA_CH_AUTO);
        if (bus_err != ESP_OK && bus_err != ESP_ERR_INVALID_STATE) {
            ESP_LOGE("ISF15ACP4", "spi_bus_initialize failed: %s", esp_err_to_name(bus_err));
            return false;
        }

        spi_device_interface_config_t dev_cfg = {};
        dev_cfg.clock_speed_hz = bus_.clock_hz;
        dev_cfg.mode = 0;
        dev_cfg.spics_io_num = GPIO_NUM_NC;
        dev_cfg.queue_size = 4;

        const esp_err_t add_err = spi_bus_add_device(bus_.host, &dev_cfg, &spi_);
        if (add_err != ESP_OK) {
            ESP_LOGE("ISF15ACP4", "spi_bus_add_device failed: %s", esp_err_to_name(add_err));
            return false;
        }

        gpio_config_t out_cfg = {};
        out_cfg.mode = GPIO_MODE_OUTPUT;
        out_cfg.pin_bit_mask =
            (1ULL << pins_.cs) | (1ULL << pins_.dc) | (1ULL << pins_.reset) | (1ULL << pins_.vcc_enable);

        gpio_config(&out_cfg);
        gpio_set_level(pins_.cs, 1);
        gpio_set_level(pins_.dc, 1);
        gpio_set_level(pins_.reset, 1);
        gpio_set_level(pins_.vcc_enable, 0);

        if (pins_.switch_in != GPIO_NUM_NC) {
            gpio_config_t in_cfg = {};
            in_cfg.mode = GPIO_MODE_INPUT;
            in_cfg.pin_bit_mask = 1ULL << pins_.switch_in;
            in_cfg.pull_up_en = GPIO_PULLUP_ENABLE;
            gpio_config(&in_cfg);
        }

        ready_ = true;
        return true;
    }

    bool BeginCommand() noexcept {
        gpio_set_level(pins_.dc, 0);
        gpio_set_level(pins_.cs, 0);
        return true;
    }

    bool BeginData() noexcept {
        gpio_set_level(pins_.dc, 1);
        gpio_set_level(pins_.cs, 0);
        return true;
    }

    void EndTransaction() noexcept { gpio_set_level(pins_.cs, 1); }

    bool WriteBytes(const uint8_t* data, std::size_t len) noexcept {
        if (data == nullptr || len == 0) {
            return false;
        }

        spi_transaction_t txn = {};
        txn.length = len * 8;
        txn.tx_buffer = data;
        const esp_err_t err = spi_device_polling_transmit(spi_, &txn);
        return err == ESP_OK;
    }

    /// True when the pin was driven; gpio_set_level() returns ESP_ERR_INVALID_ARG for a bad pin.
    bool GpioSet(isf15acp4::CtrlPin pin, isf15acp4::GpioSignal signal) noexcept {
        const bool active = signal == isf15acp4::GpioSignal::Active;
        switch (pin) {
            case isf15acp4::CtrlPin::ChipSelect:
                return gpio_set_level(pins_.cs, active ? 0 : 1) == ESP_OK;
            case isf15acp4::CtrlPin::DataCommand:
                return gpio_set_level(pins_.dc, active ? 1 : 0) == ESP_OK;
            case isf15acp4::CtrlPin::Reset:
                return gpio_set_level(pins_.reset, active ? 0 : 1) == ESP_OK;
            case isf15acp4::CtrlPin::VccEnable:
                return gpio_set_level(pins_.vcc_enable, active ? 1 : 0) == ESP_OK;
            case isf15acp4::CtrlPin::Switch:
                break;
        }
        return true;
    }

    bool GpioRead(isf15acp4::CtrlPin pin) noexcept {
        if (pin != isf15acp4::CtrlPin::Switch || pins_.switch_in == GPIO_NUM_NC) {
            return false;
        }
        const int level = gpio_get_level(pins_.switch_in);
        return pins_.switch_active_low ? (level == 0) : (level == 1);
    }

    void DelayUs(uint32_t us) noexcept {
        if (us >= 1000) {
            vTaskDelay(pdMS_TO_TICKS((us + 999) / 1000));
        } else {
            esp_rom_delay_us(us);
        }
    }

    void Deinitialize() noexcept {
        if (spi_ != nullptr) {
            spi_bus_remove_device(spi_);
            spi_ = nullptr;
        }
        ready_ = false;
    }

    [[nodiscard]] bool IsReady() const noexcept { return ready_; }

private:
    SpiBusConfig bus_;
    PinConfig pins_;
    spi_device_handle_t spi_{nullptr};
    bool ready_{false};
};

}  // namespace esp32_isf15acp4
