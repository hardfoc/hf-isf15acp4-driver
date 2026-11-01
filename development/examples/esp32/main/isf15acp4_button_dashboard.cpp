/**
 * @file isf15acp4_button_dashboard.cpp
 * @brief Motor-style dashboard with gauge, debounced button, and graphics backend selection.
 */

#include <cstdio>

#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp32_isf15acp4_setup.hpp"
#include "isf15acp4.hpp"
#include "isf15acp4_button_ui.hpp"

namespace {
constexpr const char* kTag = "ISF15ACP4_DASH";
}

extern "C" void app_main(void) {
    esp32_isf15acp4::Esp32SpiAdapter adapter(esp32_isf15acp4::default_bus_config(),
                                             esp32_isf15acp4::default_pin_config());

    const isf15acp4::SmartDisplayConfig config =
        esp32_isf15acp4::default_display_config(isf15acp4::GraphicsBackend::BuiltinCanvas);

    isf15acp4::SmartDisplay display(&adapter, config);
    isf15acp4::ButtonUI button;
    isf15acp4::GraphicsContext gfx = display.CreateGraphicsContext();

    if (!display.Initialize(true)) {
        ESP_LOGE(kTag, "Init failed");
        return;
    }

    bool running = false;
    float rpm = 0.0f;
    int64_t last_us = esp_timer_get_time();

    ESP_LOGI(kTag, "Dashboard ready — short press start/stop, long press reset RPM");

    while (true) {
        const int64_t now = esp_timer_get_time();
        const uint32_t elapsed_ms = static_cast<uint32_t>((now - last_us) / 1000);
        last_us = now;

        const auto event = button.Update(display.IsPressed(), elapsed_ms);
        if (event == isf15acp4::ButtonEvent::Click) {
            running = !running;
            ESP_LOGI(kTag, "%s", running ? "RUN" : "STOP");
        } else if (event == isf15acp4::ButtonEvent::LongPress) {
            rpm = 0.0f;
            running = false;
            ESP_LOGI(kTag, "RPM reset");
        }

        if (running) {
            rpm += 0.35f;
            if (rpm > 60.0f) {
                rpm = 0.0f;
            }
        }

        gfx.Clear(isf15acp4::colors::Black);
        gfx.FillRect({0, 0, gfx.Width(), 12}, isf15acp4::colors::Blue);
        gfx.DrawString(4, 2, "MOTOR CTRL", isf15acp4::colors::White);

        isf15acp4::ButtonUI::DrawGauge(gfx, 48, 40, 28, rpm / 60.0f, isf15acp4::colors::Green,
                                       isf15acp4::colors::Gray);

        char rpm_text[16];
        snprintf(rpm_text, sizeof(rpm_text), "%.1f", rpm);
        gfx.DrawString(34, 28, rpm_text, isf15acp4::colors::White);
        gfx.DrawString(70, 28, "RPM", isf15acp4::colors::Cyan);

        gfx.DrawString(6, 52, running ? "RUN" : "STOP", running ? isf15acp4::colors::Green
                                                                  : isf15acp4::colors::Orange);
        gfx.DrawString(50, 52, display.IsPressed() ? "HOLD" : "----",
                       display.IsPressed() ? isf15acp4::colors::Yellow : isf15acp4::colors::Gray);

        display.Present(gfx);
        vTaskDelay(pdMS_TO_TICKS(33));
    }
}
