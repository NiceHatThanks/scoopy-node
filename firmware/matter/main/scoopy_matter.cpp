#include "scoopy_matter.hpp"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <platform/CHIPDeviceLayer.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

namespace scoopy::matter {
namespace {

const char *TAG = "scoopy_matter";
uint16_t sButton1EndpointId = 0;

constexpr uint8_t kIdlePosition = 0;
constexpr uint8_t kPressedPosition = 1;

bool checkFeatureResult(esp_err_t err, const char *featureName)
{
    if (err == ESP_OK) {
        return true;
    }

    ESP_LOGE(TAG, "Failed to add %s feature to Button 1: %s", featureName, esp_err_to_name(err));
    return false;
}

void setCurrentPosition(uint8_t position)
{
    esp_matter_attr_val_t value(position);
    const esp_err_t err = attribute::update(
        sButton1EndpointId, Switch::Id, Switch::Attributes::CurrentPosition::Id, &value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Button 1 CurrentPosition update failed: %s", esp_err_to_name(err));
    }
}

void logEventResult(const char *eventName, esp_err_t err)
{
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Button 1 Matter event: %s", eventName);
    } else {
        ESP_LOGE(TAG, "Button 1 Matter event %s failed: %s", eventName, esp_err_to_name(err));
    }
}

void sendButtonAction(buttons::ButtonAction action)
{
    if (sButton1EndpointId == 0) {
        ESP_LOGE(TAG, "Button 1 endpoint is not initialized");
        return;
    }

    switch (action) {
    case buttons::ButtonAction::SinglePress:
        setCurrentPosition(kIdlePosition);
        logEventResult(
            "multi_press_1",
            cluster::switch_cluster::event::send_multi_press_complete(
                sButton1EndpointId, kPressedPosition, 1));
        break;

    case buttons::ButtonAction::DoublePress:
        setCurrentPosition(kIdlePosition);
        logEventResult(
            "multi_press_2",
            cluster::switch_cluster::event::send_multi_press_complete(
                sButton1EndpointId, kPressedPosition, 2));
        break;

    case buttons::ButtonAction::Hold:
        setCurrentPosition(kPressedPosition);
        logEventResult(
            "long_press",
            cluster::switch_cluster::event::send_long_press(
                sButton1EndpointId, kPressedPosition));
        break;

    case buttons::ButtonAction::HoldRelease:
        setCurrentPosition(kIdlePosition);
        logEventResult(
            "long_release",
            cluster::switch_cluster::event::send_long_release(
                sButton1EndpointId, kPressedPosition));
        break;
    }
}

} // namespace

bool createButton1Endpoint(node_t *node)
{
    if (node == nullptr) {
        ESP_LOGE(TAG, "Cannot create Button 1 endpoint without a Matter node");
        return false;
    }

    generic_switch::config_t switchConfig;
    switchConfig.switch_cluster.feature_flags =
        cluster::switch_cluster::feature::momentary_switch::get_id();

    endpoint_t *endpoint = generic_switch::create(node, &switchConfig, ENDPOINT_FLAG_NONE, nullptr);
    if (endpoint == nullptr) {
        ESP_LOGE(TAG, "Failed to create Button 1 Generic Switch endpoint");
        return false;
    }

    cluster_t *switchCluster = cluster::get(endpoint, Switch::Id);
    if (switchCluster == nullptr) {
        ESP_LOGE(TAG, "Failed to find Switch cluster on Button 1 endpoint");
        return false;
    }

    if (!checkFeatureResult(
            cluster::switch_cluster::feature::action_switch::add(switchCluster), "ActionSwitch")) {
        return false;
    }

    // ActionSwitch + MomentarySwitchLongPress is the Matter-standard feature
    // combination Home Assistant uses to expose long_press/long_release.
    if (!checkFeatureResult(
            cluster::switch_cluster::feature::momentary_switch_long_press::add(switchCluster),
            "MomentarySwitchLongPress")) {
        return false;
    }

    cluster::switch_cluster::feature::momentary_switch_multi_press::config_t multiPressConfig;
    multiPressConfig.multi_press_max = 2;
    if (!checkFeatureResult(
            cluster::switch_cluster::feature::momentary_switch_multi_press::add(
                switchCluster, &multiPressConfig),
            "MomentarySwitchMultiPress")) {
        return false;
    }

    sButton1EndpointId = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Button 1 Generic Switch created on endpoint %u", sButton1EndpointId);
    return true;
}

uint16_t button1EndpointId()
{
    return sButton1EndpointId;
}

void handleButton1Action(buttons::ButtonAction action, void *)
{
    // The button task is deliberately Matter-agnostic. Marshal all cluster
    // operations onto the CHIP system layer before touching the data model.
    chip::DeviceLayer::SystemLayer().ScheduleLambda([action]() {
        sendButtonAction(action);
    });
}

} // namespace scoopy::matter
