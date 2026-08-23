#pragma once
#include "esphome/core/defines.h"
#ifdef USE_CAPTIVE_PORTAL
#include <memory>
#if defined(USE_ESP32)
#include "dns_server_esp32_idf.h"
#elif defined(USE_ARDUINO)
#include <DNSServer.h>
#endif
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include "esphome/core/preferences.h"
#include "esphome/components/web_server_base/web_server_base.h"

namespace esphome::captive_portal {

class CaptivePortal final : public AsyncWebHandler, public Component {
 public:
  CaptivePortal(web_server_base::WebServerBase *base);
  void setup() override;
  void dump_config() override;
  void loop() override {
#if defined(USE_ESP32)
    if (this->dns_server_ != nullptr) {
      this->dns_server_->process_next_request();
    }
#elif defined(USE_ARDUINO)
    if (this->dns_server_ != nullptr) {
      this->dns_server_->processNextRequest();
    }
#endif
  }
  float get_setup_priority() const override;
  void start();
  bool is_active() const { return this->active_; }
  void end() {
    this->active_ = false;
    this->disable_loop();
    this->base_->deinit();
    if (this->dns_server_ != nullptr) {
      this->dns_server_->stop();
      this->dns_server_ = nullptr;
    }
  }

  bool canHandle(AsyncWebServerRequest *request) const override {
    return this->active_ && request->method() == HTTP_GET;
  }

  void handle_config(AsyncWebServerRequest *request);
  void handle_wifisave(AsyncWebServerRequest *request);
  void handle_wifi_status(AsyncWebServerRequest *request);
  void handleRequest(AsyncWebServerRequest *req) override;

 protected:
  web_server_base::WebServerBase *base_;
  bool initialized_{false};
  bool active_{false};
#if defined(USE_ARDUINO) || defined(USE_ESP32)
  std::unique_ptr<DNSServer> dns_server_{nullptr};
#endif
};

extern CaptivePortal *global_captive_portal;

}  // namespace esphome::captive_portal

#endif
