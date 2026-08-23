#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <esp_matter.h>
#include <esp_matter_console.h>
#include <common_macros.h>

static const char *TAG = "scoopy_matter";
static uint16_t button_1_endpoint_id = 0;

using namespace esp_matter;
using namespace esp_matter::attribute;
using namespace esp_matter::endpoint;
using namespace chip::app::Clusters;

static void app_event_cb(const ChipDeviceEvent *event, intptr_t arg)
{
    switch (event->Type) {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
        ESP_LOGI(TAG, "Interface IP address changed");
        break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning complete");
        break;
    case chip::DeviceLayer::DeviceEventType::kFailSafeTimerExpired:
        ESP_LOGW(TAG, "Commissioning failed: fail-safe timer expired");
        break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted:
        ESP_LOGI(TAG, "Commissioning session started");
        break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped:
        ESP_LOGI(TAG, "Commissioning session stopped");
        break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened:
        ESP_LOGI(TAG, "Commissioning window opened");
        break;
    case chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed:
        ESP_LOGI(TAG, "Commissioning window closed");
        break;
    default:
        break;
    }
}

static esp_err_t app_identification_cb(identification::callback_type_t type,
                                       uint16_t endpoint_id,
                                       uint8_t effect_id,
                                       uint8_t effect_variant,
                                       void *priv_data)
{
    ESP_LOGI(TAG, "Identify callback: endpoint=%u type=%u effect=%u variant=%u",
             endpoint_id, type, effect_id, effect_variant);
    return ESP_OK;
}

static esp_err_t app_attribute_update_cb(attribute::callback_type_t type,
                                         uint16_t endpoint_id,
                                         uint32_t cluster_id,
                                         uint32_t attribute_id,
                                         esp_matter_attr_val_t *val,
                                         void *priv_data)
{
    // Milestone 1 has no writable hardware-backed attributes yet.
    return ESP_OK;
}

static endpoint_t *create_button_1_endpoint(node_t *node)
{
    generic_switch::config_t switch_config;
    switch_config.switch_cluster.feature_flags =
        cluster::switch_cluster::feature::momentary_switch::get_id();

    endpoint_t *endpoint = generic_switch::create(node, &switch_config, ENDPOINT_FLAG_NONE, nullptr);
    if (endpoint == nullptr) {
        ESP_LOGE(TAG, "Failed to create Button 1 Generic Switch endpoint");
        return nullptr;
    }

    cluster_t *switch_cluster = cluster::get(endpoint, Switch::Id);
    if (switch_cluster == nullptr) {
        ESP_LOGE(TAG, "Failed to find Switch cluster on Button 1 endpoint");
        return nullptr;
    }

    // Match Espressif's standard momentary Generic Switch model. GPIO event
    // generation is intentionally added only after commissioning is proven.
    cluster::switch_cluster::feature::action_switch::add(switch_cluster);

    cluster::switch_cluster::feature::momentary_switch_multi_press::config_t multi_press_config;
    multi_press_config.multi_press_max = 2;
    cluster::switch_cluster::feature::momentary_switch_multi_press::add(
        switch_cluster, &multi_press_config);

    button_1_endpoint_id = endpoint::get_id(endpoint);
    ESP_LOGI(TAG, "Button 1 Generic Switch created on endpoint %u", button_1_endpoint_id);

    return endpoint;
}

extern "C" void app_main()
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "Starting experimental Scoopy Matter firmware");

    node::config_t node_config;
    node_t *node = node::create(&node_config, app_attribute_update_cb, app_identification_cb);
    ABORT_APP_ON_FAILURE(node != nullptr, ESP_LOGE(TAG, "Failed to create Matter node"));

    endpoint_t *button_1_endpoint = create_button_1_endpoint(node);
    ABORT_APP_ON_FAILURE(button_1_endpoint != nullptr,
                         ESP_LOGE(TAG, "Failed to create Button 1 endpoint"));

    err = esp_matter::start(app_event_cb);
    ABORT_APP_ON_FAILURE(err == ESP_OK,
                         ESP_LOGE(TAG, "Failed to start Matter, err=%d", err));

#if CONFIG_ENABLE_CHIP_SHELL
    esp_matter::console::diagnostics_register_commands();
    esp_matter::console::wifi_register_commands();
    esp_matter::console::factoryreset_register_commands();
    esp_matter::console::init();
#endif

    ESP_LOGI(TAG, "Matter started; Button 1 endpoint is %u", button_1_endpoint_id);
}
