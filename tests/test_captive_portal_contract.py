from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[1]
CPP = (ROOT / "firmware/esphome/components/captive_portal/captive_portal.cpp").read_text(encoding="utf-8")
EMBEDDED = (ROOT / "firmware/esphome/components/captive_portal/scoopy_index.h").read_text(encoding="utf-8")
SOURCE_HTML = (ROOT / "firmware/captive-portal/index.html").read_text(encoding="utf-8")


def test_wifi_status_endpoint_reports_connection_state():
    assert '"/wifi-status"' in CPP
    assert "handle_wifi_status" in CPP
    assert "is_connected()" in CPP
    assert '"connected"' in CPP


def test_unknown_captive_requests_redirect_to_portal_root():
    assert "req->redirect(" in CPP
    assert "wifi_soft_ap_ip()" in CPP
    assert re.search(r'url\s*==\s*ESPHOME_F\("/"\)', CPP)


def test_setup_ui_times_out_and_allows_retry():
    for frontend in (SOURCE_HTML, EMBEDDED):
        assert "/wifi-status" in frontend
        assert "30000" in frontend
        assert "Connect Scoopy" in frontend
        assert "Check the network name and password" in frontend
        assert "disabled = false" in frontend or "disabled=false" in frontend
