#include "src/apps/network/NetworkApp.h"
#include "src/core/SysNetworkManager.h"
#include "src/core/SettingsManager.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include "src/system_config.h"

void NetworkApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.drawCircle(cx, cy + 4, 2, color);
    tft.drawArc(cx, cy + 4, 6, 6, 225, 315, color, color);
    tft.drawArc(cx, cy + 4, 10, 10, 225, 315, color, color);
    tft.drawArc(cx, cy + 4, 14, 14, 225, 315, color, color);
}

const AppInfo NetworkApp::_info = { 2, "Network", NetworkApp::drawIcon };

NetworkApp::NetworkApp() {}

void NetworkApp::onCreate() {
    LOG_INFO("NetworkApp: onCreate");
}

void NetworkApp::onEnter() {
    _needsRedraw = true;
    rebuildMenu();
}

void NetworkApp::onExit() {
}

void NetworkApp::rebuildMenu() {
    _menu.clear();
    
    SysNetworkManager& net = SysNetworkManager::instance();
    if (!net.isEnabled()) {
        _menu.addItem("Wi-Fi Disabled", "");
        _menu.addItem("Enable in Settings", "");
    } else {
        if (net.isConnected()) {
            _menu.addItem("Connected:", net.getSSID().c_str());
            _menu.addItem("IP:", net.getLocalIPStr().c_str());
            char rssiStr[16];
            snprintf(rssiStr, sizeof(rssiStr), "%d dBm", (int)net.getRSSI());
            _menu.addItem("RSSI:", rssiStr);
        } else {
            _menu.addItem("Not connected", "");
        }
        _menu.addItem("Scan Networks", "");
    }
}

void NetworkApp::update(uint32_t /*deltaMs*/) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
}

void NetworkApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    // Header bar
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
    tft.setTextColor(c.primary);
    
    const char* title = _scanning ? "Scanning..." : "Network";
    int16_t tw = tft.textWidth(title, 1);
    tft.drawString(title, (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 4, 1);
    tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);

    // Render list
    _menu.render(tft, STATUS_BAR_HEIGHT + 19, CONTENT_HEIGHT - 19);
}

void NetworkApp::handleInput(AppEvent e) {
    switch (e) {
        case AppEvent::BTN_UP_SHORT:
        case AppEvent::BTN_UP_HELD:
            _menu.selectPrev();
            _needsRedraw = true;
            break;
            
        case AppEvent::BTN_DOWN_SHORT:
        case AppEvent::BTN_DOWN_HELD:
            _menu.selectNext();
            _needsRedraw = true;
            break;
            
        case AppEvent::BTN_SELECT_SHORT: {
            uint8_t sel = _menu.getSelected();
            SysNetworkManager& net = SysNetworkManager::instance();
            if (net.isEnabled() && sel == _menu.getCount() - 1) { // Scan button
                _scanning = true;
                _needsRedraw = true;
                // Wait for render to show "Scanning..."
                // Since this is blocking, we can't easily wait for render if we block right here, but for Phase 3 we'll just scan.
                int n = net.scanNetworks();
                _scanning = false;
                _menu.clear();
                if (n == 0) {
                    _menu.addItem("No networks found", "");
                } else {
                    for (int i = 0; i < n && i < MENU_MAX_ITEMS - 1; ++i) {
                        char rssi[16];
                        snprintf(rssi, sizeof(rssi), "%d", WiFi.RSSI(i));
                        _menu.addItem(WiFi.SSID(i).substring(0, 16).c_str(), rssi);
                    }
                }
                _menu.addItem("Back", "");
                _needsRedraw = true;
            } else if (sel == _menu.getCount() - 1 && !_scanning) {
                // If it's the "Back" button
                rebuildMenu();
                _needsRedraw = true;
            }
            break;
        }
            
        case AppEvent::BTN_SELECT_LONG:
            requestExit = true;
            break;
            
        default:
            break;
    }
}
