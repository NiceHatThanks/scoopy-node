"""Regression contract for Scoopy's first-time Wi-Fi captive portal."""

from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[1]
CPP = (ROOT / "firmware/esphome/components/captive_portal/captive_portal.cpp").read_text(encoding="utf-8")
EMBEDDED = (ROOT / "firmware/esphome/components/captive_portal/scoopy_index.h").read_text(encoding="utf-8")
SOURCE_HTML = (ROOT / "firmware/captive-portal/index.html").read_text(encoding="utf-8")


class CaptivePortalContractTests(unittest.TestCase):
    def test_wifi_status_endpoint_reports_connection_state(self):
        self.assertIn('"/wifi-status"', CPP)
        self.assertIn("handle_wifi_status", CPP)
        self.assertIn("is_connected()", CPP)
        self.assertIn('"connected"', CPP)

    def test_unknown_captive_requests_redirect_to_portal_root(self):
        self.assertIn("req->redirect(", CPP)
        self.assertIn("wifi_soft_ap_ip()", CPP)
        self.assertRegex(CPP, r'url\s*==\s*ESPHOME_F\("/"\)')

    def test_dhcp_advertises_captive_portal_uri(self):
        self.assertIn("ESP_NETIF_CAPTIVEPORTAL_URI", CPP)
        self.assertIn("esp_netif_dhcps_option", CPP)
        self.assertIn("esp_netif_dhcps_stop", CPP)
        self.assertIn("esp_netif_dhcps_start", CPP)

    def test_setup_ui_times_out_and_allows_retry(self):
        for frontend in (SOURCE_HTML, EMBEDDED):
            self.assertIn("/wifi-status", frontend)
            self.assertIn("30000", frontend)
            self.assertIn("Connect Scoopy", frontend)
            self.assertIn("Check the network name and password", frontend)
            self.assertTrue("disabled = false" in frontend or "disabled=false" in frontend)

    def test_setup_ui_keeps_scoopy_connecting_message(self):
        for frontend in (SOURCE_HTML, EMBEDDED):
            self.assertIn("impressively quick reader", frontend)
            self.assertIn("Scoopy is having a little think", frontend)


if __name__ == "__main__":
    unittest.main()
