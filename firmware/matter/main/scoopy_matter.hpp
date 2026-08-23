#pragma once

#include "scoopy_buttons.hpp"

#include <cstdint>
#include <esp_matter.h>

namespace scoopy::matter {

bool createButton1Endpoint(esp_matter::node_t *node);
uint16_t button1EndpointId();
void handleButton1Action(buttons::ButtonAction action, void *context);

} // namespace scoopy::matter
