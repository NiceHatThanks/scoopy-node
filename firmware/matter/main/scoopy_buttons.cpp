#include "scoopy_buttons.hpp"

namespace scoopy::buttons {
namespace {

constexpr uint32_t kDebounceMs = 20;
constexpr uint32_t kShortPressMaxMs = 500;
constexpr uint32_t kDoublePressGapMaxMs = 400;
constexpr uint32_t kSinglePressConfirmMs = 450;
constexpr uint32_t kDoublePressConfirmMs = 200;
constexpr uint32_t kHoldMs = 800;

uint32_t elapsedMs(uint32_t nowMs, uint32_t sinceMs)
{
    return nowMs - sinceMs;
}

} // namespace

std::optional<ButtonAction> ButtonGestureDetector::handleDebouncedEdge(bool pressed, uint32_t nowMs)
{
    if (pressed) {
        switch (state_) {
        case State::Idle:
            pressStartedAtMs_ = nowMs;
            state_ = State::PressedFirst;
            break;

        case State::WaitSecondPress:
            if (elapsedMs(nowMs, releaseStartedAtMs_) <= kDoublePressGapMaxMs) {
                pressStartedAtMs_ = nowMs;
                state_ = State::PressedSecond;
            } else {
                // Too late for a double press. Treat this as a fresh first press.
                pressStartedAtMs_ = nowMs;
                state_ = State::PressedFirst;
            }
            break;

        case State::WaitDoubleConfirm:
            // MultiPressMax is two. A third press before confirmation starts a
            // fresh sequence rather than emitting a misleading double event.
            pressStartedAtMs_ = nowMs;
            state_ = State::PressedFirst;
            break;

        case State::PressedFirst:
        case State::PressedSecond:
        case State::HoldActive:
            break;
        }
        return std::nullopt;
    }

    switch (state_) {
    case State::PressedFirst:
        if (elapsedMs(nowMs, pressStartedAtMs_) <= kShortPressMaxMs) {
            releaseStartedAtMs_ = nowMs;
            state_ = State::WaitSecondPress;
        } else {
            state_ = State::Idle;
        }
        break;

    case State::PressedSecond:
        if (elapsedMs(nowMs, pressStartedAtMs_) <= kShortPressMaxMs) {
            releaseStartedAtMs_ = nowMs;
            state_ = State::WaitDoubleConfirm;
        } else {
            state_ = State::Idle;
        }
        break;

    case State::HoldActive:
        state_ = State::Idle;
        return ButtonAction::HoldRelease;

    case State::Idle:
    case State::WaitSecondPress:
    case State::WaitDoubleConfirm:
        break;
    }

    return std::nullopt;
}

std::optional<ButtonAction> ButtonGestureDetector::update(bool rawPressed, uint32_t nowMs)
{
    if (!initialized_) {
        initialized_ = true;
        rawPressed_ = rawPressed;
        rawChangedAtMs_ = nowMs;
    } else if (rawPressed != rawPressed_) {
        rawPressed_ = rawPressed;
        rawChangedAtMs_ = nowMs;
    }

    if (rawPressed_ != debouncedPressed_ && elapsedMs(nowMs, rawChangedAtMs_) >= kDebounceMs) {
        debouncedPressed_ = rawPressed_;
        if (auto action = handleDebouncedEdge(debouncedPressed_, nowMs); action.has_value()) {
            return action;
        }
    }

    if (debouncedPressed_) {
        if ((state_ == State::PressedFirst || state_ == State::PressedSecond) &&
            elapsedMs(nowMs, pressStartedAtMs_) >= kHoldMs) {
            state_ = State::HoldActive;
            return ButtonAction::Hold;
        }
        return std::nullopt;
    }

    if (state_ == State::WaitSecondPress &&
        elapsedMs(nowMs, releaseStartedAtMs_) >= kSinglePressConfirmMs) {
        state_ = State::Idle;
        return ButtonAction::SinglePress;
    }

    if (state_ == State::WaitDoubleConfirm &&
        elapsedMs(nowMs, releaseStartedAtMs_) >= kDoublePressConfirmMs) {
        state_ = State::Idle;
        return ButtonAction::DoublePress;
    }

    return std::nullopt;
}

} // namespace scoopy::buttons

#ifdef ESP_PLATFORM

#include <driver/gpio.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace scoopy::buttons {
namespace {

constexpr gpio_num_t kButton1Gpio = GPIO_NUM_2;
constexpr uint32_t kButtonPollMs = 10;
constexpr uint32_t kButtonTaskStackSize = 3072;
constexpr UBaseType_t kButtonTaskPriority = 5;

const char *TAG = "scoopy_buttons";
ButtonActionCallback sButton1Callback = nullptr;
void *sButton1Context = nullptr;

const char *actionName(ButtonAction action)
{
    switch (action) {
    case ButtonAction::SinglePress:
        return "single";
    case ButtonAction::DoublePress:
        return "double";
    case ButtonAction::Hold:
        return "hold";
    case ButtonAction::HoldRelease:
        return "hold-release";
    }
    return "unknown";
}

void button1Task(void *)
{
    ButtonGestureDetector detector;

    while (true) {
        const bool rawPressed = gpio_get_level(kButton1Gpio) == 0;
        const uint32_t nowMs = static_cast<uint32_t>(esp_timer_get_time() / 1000ULL);

        if (auto action = detector.update(rawPressed, nowMs); action.has_value()) {
            ESP_LOGI(TAG, "Button 1: %s", actionName(*action));
            if (sButton1Callback != nullptr) {
                sButton1Callback(*action, sButton1Context);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(kButtonPollMs));
    }
}

} // namespace

bool initButton1(ButtonActionCallback callback, void *context)
{
    if (callback == nullptr) {
        ESP_LOGE(TAG, "Button 1 callback is null");
        return false;
    }

    gpio_config_t config = {};
    config.pin_bit_mask = 1ULL << static_cast<uint32_t>(kButton1Gpio);
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = GPIO_PULLUP_ENABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;

    const esp_err_t err = gpio_config(&config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure Button 1 GPIO2: %s", esp_err_to_name(err));
        return false;
    }

    sButton1Callback = callback;
    sButton1Context = context;

    if (xTaskCreate(button1Task, "scoopy_button1", kButtonTaskStackSize, nullptr,
                    kButtonTaskPriority, nullptr) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create Button 1 task");
        sButton1Callback = nullptr;
        sButton1Context = nullptr;
        return false;
    }

    ESP_LOGI(TAG, "Button 1 initialized on GPIO2 (active-low, pull-up, 20ms debounce)");
    return true;
}

} // namespace scoopy::buttons

#endif // ESP_PLATFORM
