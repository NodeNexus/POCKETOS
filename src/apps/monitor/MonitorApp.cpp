#include "src/apps/monitor/MonitorApp.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include "src/core/SysNetworkManager.h"
#include <HTTPClient.h>

void MonitorApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    // Draw a simple server/raspberry pi icon
    tft.drawRect(cx - 6, cy - 6, 12, 12, color);
    tft.fillRect(cx - 3, cy - 3, 2, 2, color);
    tft.fillRect(cx + 1, cy - 3, 2, 2, color);
    tft.fillRect(cx - 3, cy + 1, 6, 2, color);
}

const AppInfo MonitorApp::_info = { 3, "Monitor", MonitorApp::drawIcon };

MonitorApp::MonitorApp() {}

void MonitorApp::onCreate() {
    LOG_INFO("MonitorApp: onCreate");
}

void MonitorApp::onEnter() {
    _state = MonitorState::LOADING;
    _needsRedraw = true;
    fetchStatus();
}

void MonitorApp::onExit() {
}

void MonitorApp::fetchStatus() {
    if (!SysNetworkManager::instance().isConnected()) {
        _state = MonitorState::ERROR;
        return;
    }
    
    HTTPClient http;
    // Default mock endpoint for a local Raspberry Pi running a python status script
    String url = "http://192.168.1.100:5000/status";
    http.begin(url);
    http.setTimeout(2000); // short timeout so it doesn't block forever if unreachable
    int httpCode = http.GET();
    
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);
            
            if (!error) {
                _cpuLoad = doc["cpu"]; // expecting percentage 0-100
                _ramUsage = doc["ram"]; // expecting MB used
                _temperature = doc["temp"]; // expecting celsius
                _state = MonitorState::LOADED;
            } else {
                _state = MonitorState::ERROR;
            }
        } else {
            _state = MonitorState::ERROR;
        }
    } else {
        _state = MonitorState::ERROR;
    }
    http.end();
    _needsRedraw = true;
}

void MonitorApp::update(uint32_t /*deltaMs*/) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
}

void MonitorApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
    tft.setTextColor(c.primary);
    int16_t tw = tft.textWidth("Pi Monitor", 1);
    tft.drawString("Pi Monitor", (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 4, 1);
    tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);

    if (_state == MonitorState::LOADING) {
        tft.setTextColor(c.textMuted);
        tft.drawString("Connecting...", 20, 60, 1);
    } else if (_state == MonitorState::ERROR) {
        tft.setTextColor(c.error);
        tft.drawString("Connection Failed.", 10, 50, 1);
        tft.setTextColor(c.textMuted);
        tft.drawString("Ensure Pi is up at", 10, 70, 1);
        tft.drawString("192.168.1.100:5000", 10, 85, 1);
    } else if (_state == MonitorState::LOADED) {
        tft.setTextColor(c.text);
        
        char cpuStr[16];
        snprintf(cpuStr, sizeof(cpuStr), "CPU: %.1f%%", _cpuLoad);
        tft.drawString(cpuStr, 10, 45, 1);
        
        char ramStr[16];
        snprintf(ramStr, sizeof(ramStr), "RAM: %.0f MB", _ramUsage);
        tft.drawString(ramStr, 10, 65, 1);
        
        char tempStr[16];
        snprintf(tempStr, sizeof(tempStr), "Temp: %.1f C", _temperature);
        tft.drawString(tempStr, 10, 85, 1);
    }
}

void MonitorApp::handleInput(AppEvent e) {
    if (e == AppEvent::BTN_SELECT_LONG) {
        requestExit = true;
    } else if (e == AppEvent::BTN_SELECT_SHORT) {
        _state = MonitorState::LOADING;
        _needsRedraw = true;
        fetchStatus();
    }
}
