#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/ui/Widgets.h"
#include "src/core/TimeManager.h"
#include "src/core/SysNetworkManager.h"
#include "src/core/StorageManager.h"

StatusBar& StatusBar::instance() {
    static StatusBar inst;
    return inst;
}

void StatusBar::init() {
    _dirty       = true;
    _lastUpdateMs = 0;
}

void StatusBar::update() {
    uint32_t now = millis();
    if (now - _lastUpdateMs < 1000) return;
    _lastUpdateMs = now;

    // Time string
    TimeManager::instance().getTimeString(_timeStr, sizeof(_timeStr));

    // Wi-Fi
    auto& net = SysNetworkManager::instance();
    bool  was  = _wifiConnected;
    _wifiConnected = net.isConnected();
    if (_wifiConnected) {
        int32_t rssi    = net.getRSSI();
        _wifiStrength   = (rssi >= -50) ? 4 :
                          (rssi >= -60) ? 3 :
                          (rssi >= -70) ? 2 :
                          (rssi >= -80) ? 1 : 0;
    } else {
        _wifiStrength = 0;
    }

    // SD
    bool sdWas = _sdPresent;
    _sdPresent = StorageManager::instance().isAvailable();

    // Mark dirty if anything changed
    if (_wifiConnected != was || _sdPresent != sdWas) _dirty = true;
    else _dirty = true;   // Always redraw time (seconds-based)
}

void StatusBar::render(TFT_eSPI& tft) {
    if (!_dirty) return;
    _dirty = false;

    const ThemeColors& c = Theme::instance().colors();

    // Background + bottom border
    tft.fillRect(0, 0, SCREEN_WIDTH, HEIGHT - 1, c.statusBar);
    tft.drawFastHLine(0, HEIGHT - 1, SCREEN_WIDTH, c.border);

    // Time — left-aligned, font 1 (GLCD 6×8)
    tft.setTextColor(c.statusBarText);
    tft.drawString(_timeStr, 3, 2, 1);

    // Right-side icons (build right-to-left)
    int16_t rx = SCREEN_WIDTH - 3;

    // SD icon (8×10 → fits in 12 px wide slot)
    rx -= 11;
    Widgets::drawSDIcon(tft, rx, 1,
                        _sdPresent,
                        _sdPresent ? c.success : c.textDisabled);

    // Wi-Fi icon (~14 wide; use drawArc-based icon centred)
    rx -= 15;
    Widgets::drawWifiIcon(tft, rx + 7, -2,
                          _wifiConnected, _wifiStrength,
                          _wifiConnected ? c.primary : c.textDisabled);
}
