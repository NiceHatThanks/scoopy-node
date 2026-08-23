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

    def test_setup_ui_times_out_and_allows_retry(self):
        for frontend in (SOURCE_HTML, EMBEDDED):
            self.assertIn("/wifi-status", frontend)
            self.assertIn("30000", frontend)
            self.assertIn("Connect Scoopy", frontend)
            self.assertIn("Check the network name and password", frontend)
            self.assertTrue("disabled = false" in frontend or "disabled=false" in frontend)


if __name__ == "__main__":
    unittest.main()
