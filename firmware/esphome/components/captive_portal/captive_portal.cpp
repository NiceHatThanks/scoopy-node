#include "captive_portal.h"
#ifdef USE_CAPTIVE_PORTAL
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/wifi/wifi_component.h"
#include "json_escape.h"
#include "scoopy_index.h"

namespace esphome::captive_portal {

static const char *const TAG = "captive_portal";

void CaptivePortal::handle_config(AsyncWebServerRequest *request) {
  AsyncResponseStream *stream = request->beginResponseStream(ESPHOME_F("application/json"));
  stream->addHeader(ESPHOME_F("cache-control"), ESPHOME_F("public, max-age=0, must-revalidate"));
  char mac_s[18];
  const char *mac_str = get_mac_address_pretty_into_buffer(mac_s);
#ifdef USE_ESP8266
  stream->print(ESPHOME_F("{\"mac\":\""));
  stream->print(mac_str);
  stream->print(ESPHOME_F("\",\"name\":\""));
  stream->print(App.get_name().c_str());
  stream->print(ESPHOME_F("\",\"aps\":[{}"));
#else
  stream->printf(R"({"mac":"%s","name":"%s","aps":[{})", mac_str, App.get_name().c_str());
#endif

  char escaped_ssid[32 * JSON_ESCAPE_MAX_EXPANSION + 1];
  {
    wifi::ScanResultsLock lock(wifi::global_wifi_component);
    for (const auto &scan : wifi::global_wifi_component->get_scan_result()) {
      if (scan.get_is_hidden())
        continue;

      json_escape_into_buffer(escaped_ssid, scan.get_ssid());
#ifdef USE_ESP8266
      stream->print(ESPHOME_F(",{\"ssid\":\""));
      stream->print(escaped_ssid);
      stream->print(ESPHOME_F("\",\"rssi\":"));
      stream->print(scan.get_rssi());
      stream->print(ESPHOME_F(",\"lock\":"));
      stream->print(scan.get_with_auth());
      stream->print(ESPHOME_F("}"));
#else
      stream->printf(R"(,{"ssid":"%s","rssi":%d,"lock":%d})", escaped_ssid, scan.get_rssi(), scan.get_with_auth());
#endif
    }
  }
  stream->print(ESPHOME_F("]}"));
  request->send(stream);
}

void CaptivePortal::handle_wifisave(AsyncWebServerRequest *request) {
  const auto &ssid = request->arg("ssid");
  const auto &psk = request->arg("psk");
  ESP_LOGI(TAG,
           "Requested WiFi Settings Change:\n"
           "  SSID='%s'\n"
           "  Password=" LOG_SECRET("'%s'"),
           ssid.c_str(), psk.c_str());
#ifdef USE_ESP8266
  wifi::global_wifi_component->save_wifi_sta(ssid.c_str(), psk.c_str());
#else
  this->defer([ssid, psk]() { wifi::global_wifi_component->save_wifi_sta(ssid.c_str(), psk.c_str()); });
#endif
  request->send(200, ESPHOME_F("text/plain"), ESPHOME_F("Saved. Connecting..."));
}

void CaptivePortal::setup() {
  this->disable_loop();
}

void CaptivePortal::start() {
  this->base_->init();
  if (!this->initialized_) {
    this->base_->add_handler_without_auth(this);
  }

  network::IPAddress ip = wifi::global_wifi_component->wifi_soft_ap_ip();

#if defined(USE_ESP32)
  this->dns_server_ = make_unique<DNSServer>();
  this->dns_server_->start(ip);
#elif defined(USE_ARDUINO)
  this->dns_server_ = make_unique<DNSServer>();
  this->dns_server_->setErrorReplyCode(DNSReplyCode::NoError);
  this->dns_server_->start(53, ESPHOME_F("*"), ip);
#endif

  this->initialized_ = true;
  this->active_ = true;
  this->enable_loop();

  ESP_LOGV(TAG, "Captive portal started");
}

void CaptivePortal::handleRequest(AsyncWebServerRequest *req) {
#ifdef USE_ESP32
  char url_buf[AsyncWebServerRequest::URL_BUF_SIZE];
  StringRef url = req->url_to(url_buf);
#else
  const auto &url = req->url();
#endif

  if (url == ESPHOME_F("/config.json")) {
    this->handle_config(req);
    return;
  }

  if (url == ESPHOME_F("/wifisave")) {
    this->handle_wifisave(req);
    return;
  }

  // Preserve ESPHome captive detection behaviour: every other URL receives
  // the Scoopy setup page.
  auto *response = req->beginResponse(200, ESPHOME_F("text/html"), SCOOPY_INDEX);
  response->addHeader(ESPHOME_F("Cache-Control"), ESPHOME_F("no-store"));
  req->send(response);
}

CaptivePortal::CaptivePortal(web_server_base::WebServerBase *base) : base_(base) {
  global_captive_portal = this;
}

float CaptivePortal::get_setup_priority() const {
  return setup_priority::WIFI + 1.0f;
}

void CaptivePortal::dump_config() {
  ESP_LOGCONFIG(TAG, "Captive Portal:");
  ESP_LOGCONFIG(TAG, "  Frontend: Scoopy branded");
}

CaptivePortal *global_captive_portal = nullptr;

}  // namespace esphome::captive_portal

#endif
