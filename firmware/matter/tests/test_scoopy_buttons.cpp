#include "scoopy_buttons.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using scoopy::buttons::ButtonAction;
using scoopy::buttons::ButtonGestureDetector;

namespace {

struct ObservedAction {
    uint32_t timeMs;
    ButtonAction action;
};

std::string actionName(ButtonAction action)
{
    switch (action) {
    case ButtonAction::SinglePress:
        return "SinglePress";
    case ButtonAction::DoublePress:
        return "DoublePress";
    case ButtonAction::Hold:
        return "Hold";
    case ButtonAction::HoldRelease:
        return "HoldRelease";
    }
    return "Unknown";
}

void fail(const std::string &message)
{
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
}

void expectActions(const std::string &name,
                   const std::vector<ObservedAction> &actual,
                   const std::vector<ObservedAction> &expected)
{
    if (actual.size() != expected.size()) {
        fail(name + ": expected " + std::to_string(expected.size()) +
             " actions, got " + std::to_string(actual.size()));
    }

    for (size_t i = 0; i < expected.size(); ++i) {
        if (actual[i].action != expected[i].action || actual[i].timeMs != expected[i].timeMs) {
            fail(name + ": action " + std::to_string(i) + " expected " +
                 actionName(expected[i].action) + " at " + std::to_string(expected[i].timeMs) +
                 "ms, got " + actionName(actual[i].action) + " at " +
                 std::to_string(actual[i].timeMs) + "ms");
        }
    }
}

void tick(ButtonGestureDetector &detector,
          bool rawPressed,
          uint32_t startMs,
          uint32_t endMs,
          std::vector<ObservedAction> &actions)
{
    for (uint32_t now = startMs; now <= endMs; now += 10) {
        if (auto action = detector.update(rawPressed, now); action.has_value()) {
            actions.push_back({now, *action});
        }
    }
}

void testSinglePress()
{
    ButtonGestureDetector detector;
    std::vector<ObservedAction> actions;
    tick(detector, false, 0, 40, actions);
    tick(detector, true, 50, 140, actions);   // debounced press at 70ms
    tick(detector, false, 150, 620, actions); // debounced release at 170ms; single at 620ms
    expectActions("single press", actions, {{620, ButtonAction::SinglePress}});
}

void testDoublePress()
{
    ButtonGestureDetector detector;
    std::vector<ObservedAction> actions;
    tick(detector, false, 0, 40, actions);
    tick(detector, true, 50, 140, actions);   // press at 70
    tick(detector, false, 150, 290, actions); // release at 170
    tick(detector, true, 300, 390, actions);  // second press at 320
    tick(detector, false, 400, 620, actions); // release at 420; double at 620
    expectActions("double press", actions, {{620, ButtonAction::DoublePress}});
}

void testHoldAndRelease()
{
    ButtonGestureDetector detector;
    std::vector<ObservedAction> actions;
    tick(detector, false, 0, 40, actions);
    tick(detector, true, 50, 890, actions);   // debounced press at 70; hold at 870
    tick(detector, false, 900, 940, actions); // debounced release at 920
    expectActions("hold", actions,
                  {{870, ButtonAction::Hold}, {920, ButtonAction::HoldRelease}});
}

void testBounceUnderDebounceIsIgnored()
{
    ButtonGestureDetector detector;
    std::vector<ObservedAction> actions;
    tick(detector, false, 0, 40, actions);
    tick(detector, true, 50, 50, actions);
    tick(detector, false, 60, 700, actions); // only 10ms high
    expectActions("debounce", actions, {});
}

void testMediumPressIsNotMisclassified()
{
    ButtonGestureDetector detector;
    std::vector<ObservedAction> actions;
    tick(detector, false, 0, 40, actions);
    tick(detector, true, 50, 650, actions);
    tick(detector, false, 660, 1200, actions); // >500ms and <800ms
    expectActions("medium press", actions, {});
}

} // namespace

int main()
{
    testSinglePress();
    testDoublePress();
    testHoldAndRelease();
    testBounceUnderDebounceIsIgnored();
    testMediumPressIsNotMisclassified();
    std::cout << "All Scoopy button gesture tests passed\n";
    return 0;
}
