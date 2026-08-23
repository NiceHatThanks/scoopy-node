#pragma once

#include <cstdint>
#include <optional>

namespace scoopy::buttons {

enum class ButtonAction {
    SinglePress,
    DoublePress,
    Hold,
    HoldRelease,
};

using ButtonActionCallback = void (*)(ButtonAction action, void *context);

class ButtonGestureDetector {
public:
    std::optional<ButtonAction> update(bool rawPressed, uint32_t nowMs);

private:
    enum class State {
        Idle,
        PressedFirst,
        WaitSecondPress,
        PressedSecond,
        WaitDoubleConfirm,
        HoldActive,
    };

    bool initialized_ = false;
    bool rawPressed_ = false;
    bool debouncedPressed_ = false;
    uint32_t rawChangedAtMs_ = 0;
    uint32_t pressStartedAtMs_ = 0;
    uint32_t releaseStartedAtMs_ = 0;
    State state_ = State::Idle;

    std::optional<ButtonAction> handleDebouncedEdge(bool pressed, uint32_t nowMs);
};

// Initializes Scoopy Button 1 on active-low GPIO2 with the board's internal
// pull-up. Actions are delivered from the button polling task; consumers that
// touch Matter must marshal them onto the Matter/CHIP work context.
bool initButton1(ButtonActionCallback callback, void *context = nullptr);

} // namespace scoopy::buttons
