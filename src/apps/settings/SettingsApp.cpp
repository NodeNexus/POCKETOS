#include "src/apps/settings/SettingsApp.h"
#include "src/core/SettingsManager.h"
#include "src/core/StorageManager.h"
#include "src/core/DisplayManager.h"
#include "src/ui/StatusBar.h"
#include "src/debug.h"
#include "src/board_config.h"
#include "src/system_config.h"
#include <math.h>

// ---------------------------------------------------------------------------
// Gear icon (defined here so SettingsApp owns it)
// ---------------------------------------------------------------------------
void SettingsApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.drawCircle(cx, cy, 5, color);
    tft.fillCircle(cx, cy, 3, color);
    for (int a = 0; a < 360; a += 45) {
        float rad = (float)a * (float)M_PI / 180.0f;
        tft.drawLine(cx + (int16_t)(cosf(rad) * 6), cy + (int16_t)(sinf(rad) * 6),
                     cx + (int16_t)(cosf(rad) * 9), cy + (int16_t)(sinf(rad) * 9), color);
    }
}

const AppInfo SettingsApp::_info = { 9, "Settings", SettingsApp::drawIcon };

SettingsApp::SettingsApp() {}

void SettingsApp::onCreate() { LOG_INFO("SettingsApp: onCreate"); }

void SettingsApp::onEnter() {
    _screen      = SettingsScreen::MAIN;
    _needsRedraw = true;
    buildMain();
}

void SettingsApp::onExit() { applyAndSave(); }

void SettingsApp::applyAndSave() {
    SettingsManager::instance().save();
}

// ---------------------------------------------------------------------------
// Menu builders
// ---------------------------------------------------------------------------
void SettingsApp::buildMain() {
    _menu.clear();
    _menu.addItem("Display",  ">");
    _menu.addItem("Theme",    ">");
    _menu.addItem("Wi-Fi",    ">");
    _menu.addItem("Storage",  ">");
    _menu.addItem("About",    ">");
    _needsRedraw = true;
}

void SettingsApp::buildDisplay() {
    auto& s = SettingsManager::instance();
    char brightStr[8];
    snprintf(brightStr, sizeof(brightStr), "%d%%",
             (int)(s.getBrightness() * 100 / 255));
    _menu.clear();
    _menu.addItem("Brightness", brightStr);
    _menu.addItem("Animations", s.getAnimations() ? "ON" : "OFF");
    _menu.addItem("< Back",     "");
    _needsRedraw = true;
}

void SettingsApp::buildTheme() {
    ThemeId cur = SettingsManager::instance().getTheme();
    _menu.clear();
    _menu.addItem("Dark",   cur == ThemeId::DARK  ? "*" : " ");
    _menu.addItem("Light",  cur == ThemeId::LIGHT ? "*" : " ");
    _menu.addItem("Cyber",  cur == ThemeId::CYBER ? "*" : " ");
    _menu.addItem("< Back", "");
    _needsRedraw = true;
}

void SettingsApp::buildWifi() {
    auto& s = SettingsManager::instance();
    _menu.clear();
    _menu.addItem("Wi-Fi", s.getWifiEnabled() ? "ON" : "OFF");
    _menu.addItem("Network", s.getWifiSSID().isEmpty() ? "---" : s.getWifiSSID().c_str());
    _menu.addItem("< Back", "");
    _needsRedraw = true;
}

void SettingsApp::buildStorage() {
    auto& st = StorageManager::instance();
    _menu.clear();
    if (st.isAvailable()) {
        char buf[12];
        snprintf(buf, sizeof(buf), "%lu MB", (unsigned long)st.cardSizeMB());
        _menu.addItem("SD Card", buf);
        snprintf(buf, sizeof(buf), "%lu MB", (unsigned long)(st.usedBytes() / (1024*1024)));
        _menu.addItem("Used",    buf);
        snprintf(buf, sizeof(buf), "%lu MB", (unsigned long)(st.freeBytes() / (1024*1024)));
        _menu.addItem("Free",    buf);
    } else {
        _menu.addItem("SD Card", "None");
    }
    _menu.addItem("< Back", "");
    _needsRedraw = true;
}

// ---------------------------------------------------------------------------
// update / render
// ---------------------------------------------------------------------------
void SettingsApp::update(uint32_t /*deltaMs*/) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
}

void SettingsApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    if (_screen == SettingsScreen::ABOUT) {
        renderHeader(tft, "About");
        renderAbout(tft);
        return;
    }

    const char* titles[] = { "Settings","Display","Theme","Wi-Fi","Storage","About" };
    renderHeader(tft, titles[(uint8_t)_screen]);

    // Menu starts below header
    _menu.render(tft, STATUS_BAR_HEIGHT + 19, CONTENT_HEIGHT - 19);
}

void SettingsApp::renderHeader(TFT_eSPI& tft, const char* title) {
    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
    tft.setTextColor(c.text);
    int16_t tw = tft.textWidth(title, 2);
    tft.drawString(title, (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 2, 2);
    tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);
}

void SettingsApp::renderAbout(TFT_eSPI& tft) {
    const ThemeColors& c = Theme::instance().colors();
    int16_t y = STATUS_BAR_HEIGHT + 22;
    char buf[32];

    snprintf(buf, sizeof(buf), "PocketOS v%d.%d.%d",
             POCKETOS_VERSION_MAJOR, POCKETOS_VERSION_MINOR, POCKETOS_VERSION_PATCH);
    tft.setTextColor(c.primary);
    tft.drawString(buf, 4, y, 1); y += 12;

    tft.setTextColor(c.textMuted);
    snprintf(buf, sizeof(buf), "ESP32-S3  %lu MHz",
             (unsigned long)(ESP.getCpuFreqMHz()));
    tft.drawString(buf, 4, y, 1); y += 11;

    snprintf(buf, sizeof(buf), "Heap: %lu KB",
             (unsigned long)(ESP.getFreeHeap() / 1024));
    tft.drawString(buf, 4, y, 1); y += 11;

    snprintf(buf, sizeof(buf), "PSRAM: %lu KB",
             (unsigned long)(ESP.getFreePsram() / 1024));
    tft.drawString(buf, 4, y, 1); y += 11;

    snprintf(buf, sizeof(buf), "Flash: %lu KB",
             (unsigned long)(ESP.getFlashChipSize() / 1024));
    tft.drawString(buf, 4, y, 1); y += 11;

    snprintf(buf, sizeof(buf), "Up: %lus", (unsigned long)(millis() / 1000));
    tft.drawString(buf, 4, y, 1);

    tft.setTextColor(c.textDisabled);
    const char* hint = "SEL = Back";
    tft.drawString(hint, (SCREEN_WIDTH - tft.textWidth(hint, 1)) / 2,
                   SCREEN_HEIGHT - 11, 1);
}

// ---------------------------------------------------------------------------
// Input dispatch
// ---------------------------------------------------------------------------
void SettingsApp::handleInput(AppEvent e) {
    switch (_screen) {
        case SettingsScreen::MAIN:         handleMain(e);    break;
        case SettingsScreen::DISPLAY_MENU: handleDisplay(e); break;
        case SettingsScreen::THEME_SELECT: handleTheme(e);   break;
        case SettingsScreen::WIFI:         handleWifi(e);    break;
        case SettingsScreen::STORAGE:      handleStorage(e); break;
        case SettingsScreen::ABOUT:        handleAbout(e);   break;
    }
}

void SettingsApp::handleMain(AppEvent e) {
    switch (e) {
        case AppEvent::BTN_UP_SHORT:   _menu.selectPrev(); _needsRedraw = true; break;
        case AppEvent::BTN_DOWN_SHORT: _menu.selectNext(); _needsRedraw = true; break;
        case AppEvent::BTN_SELECT_SHORT:
            switch (_menu.getSelected()) {
                case 0: _screen = SettingsScreen::DISPLAY_MENU; buildDisplay(); break;
                case 1: _screen = SettingsScreen::THEME_SELECT; buildTheme();   break;
                case 2: _screen = SettingsScreen::WIFI;         buildWifi();    break;
                case 3: _screen = SettingsScreen::STORAGE;      buildStorage(); break;
                case 4: _screen = SettingsScreen::ABOUT; _needsRedraw = true;  break;
            }
            break;
        default: break;
    }
}

void SettingsApp::handleDisplay(AppEvent e) {
    auto& s = SettingsManager::instance();
    switch (e) {
        case AppEvent::BTN_UP_SHORT:   _menu.selectPrev(); _needsRedraw = true; break;
        case AppEvent::BTN_DOWN_SHORT: _menu.selectNext(); _needsRedraw = true; break;
        case AppEvent::BTN_SELECT_SHORT:
            switch (_menu.getSelected()) {
                case 0: {
                    // Cycle brightness presets
                    uint8_t br = s.getBrightness();
                    br = (br < 80) ? 120 : (br < 150) ? 180 :
                         (br < 210) ? 230 : (br < 252) ? 255 : 60;
                    s.setBrightness(br);
                    DisplayManager::instance().setBrightness(br);
                    buildDisplay();
                    break;
                }
                case 1:
                    s.setAnimations(!s.getAnimations());
                    buildDisplay();
                    break;
                case 2:
                    _screen = SettingsScreen::MAIN; buildMain();
                    break;
            }
            break;
        default: break;
    }
}

void SettingsApp::handleTheme(AppEvent e) {
    switch (e) {
        case AppEvent::BTN_UP_SHORT:   _menu.selectPrev(); _needsRedraw = true; break;
        case AppEvent::BTN_DOWN_SHORT: _menu.selectNext(); _needsRedraw = true; break;
        case AppEvent::BTN_SELECT_SHORT: {
            uint8_t sel = _menu.getSelected();
            if (sel <= 2) {
                ThemeId id = (ThemeId)sel;
                SettingsManager::instance().setTheme(id);
                Theme::instance().setTheme(id);
                buildTheme();       // Rebuild with updated * marker
                _needsRedraw = true;
            } else {
                _screen = SettingsScreen::MAIN; buildMain();
            }
            break;
        }
        default: break;
    }
}

void SettingsApp::handleWifi(AppEvent e) {
    switch (e) {
        case AppEvent::BTN_UP_SHORT:   _menu.selectPrev(); _needsRedraw = true; break;
        case AppEvent::BTN_DOWN_SHORT: _menu.selectNext(); _needsRedraw = true; break;
        case AppEvent::BTN_SELECT_SHORT: {
            uint8_t sel = _menu.getSelected();
            if (sel == 0) {
                bool en = !SettingsManager::instance().getWifiEnabled();
                SettingsManager::instance().setWifiEnabled(en);
                buildWifi();
            } else if (sel == _menu.getCount() - 1) {
                _screen = SettingsScreen::MAIN; buildMain();
            }
            break;
        }
        default: break;
    }
}

void SettingsApp::handleStorage(AppEvent e) {
    switch (e) {
        case AppEvent::BTN_UP_SHORT:   _menu.selectPrev(); _needsRedraw = true; break;
        case AppEvent::BTN_DOWN_SHORT: _menu.selectNext(); _needsRedraw = true; break;
        case AppEvent::BTN_SELECT_SHORT:
            if (_menu.getSelected() == _menu.getCount() - 1) {
                _screen = SettingsScreen::MAIN; buildMain();
            }
            break;
        default: break;
    }
}

void SettingsApp::handleAbout(AppEvent e) {
    if (e == AppEvent::BTN_SELECT_SHORT) {
        _screen = SettingsScreen::MAIN; buildMain();
    }
}
