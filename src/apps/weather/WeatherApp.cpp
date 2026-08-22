#include "src/apps/weather/WeatherApp.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include "src/core/SysNetworkManager.h"
#include <HTTPClient.h>

void WeatherApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    // Draw a simple cloud
    tft.fillCircle(cx - 3, cy + 2, 4, color);
    tft.fillCircle(cx + 3, cy + 2, 4, color);
    tft.fillCircle(cx, cy - 1, 4, color);
    tft.fillRect(cx - 3, cy - 2, 7, 8, color);
}

const AppInfo WeatherApp::_info = { 6, "Weather", WeatherApp::drawIcon };

WeatherApp::WeatherApp() {}

void WeatherApp::onCreate() {
    LOG_INFO("WeatherApp: onCreate");
}

void WeatherApp::onEnter() {
    _state = WeatherState::LOADING;
    _needsRedraw = true;
    fetchWeather();
}

void WeatherApp::onExit() {
}

void WeatherApp::fetchWeather() {
    if (!SysNetworkManager::instance().isConnected()) {
        _state = WeatherState::ERROR;
        return;
    }
    
    HTTPClient http;
    // Open-Meteo: London as default, fetching current temp, wind, and code
    String url = "http://api.open-meteo.com/v1/forecast?latitude=51.5085&longitude=-0.1257&current_weather=true";
    http.begin(url);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            JsonDocument doc;
            DeserializationError error = deserializeJson(doc, payload);
            
            if (!error) {
                _temperature = doc["current_weather"]["temperature"];
                _windSpeed = doc["current_weather"]["windspeed"];
                _weatherCode = doc["current_weather"]["weathercode"];
                _state = WeatherState::LOADED;
            } else {
                _state = WeatherState::ERROR;
            }
        } else {
            _state = WeatherState::ERROR;
        }
    } else {
        _state = WeatherState::ERROR;
    }
    http.end();
    _needsRedraw = true;
}

void WeatherApp::update(uint32_t /*deltaMs*/) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
}

void WeatherApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
    tft.setTextColor(c.primary);
    int16_t tw = tft.textWidth("Weather", 1);
    tft.drawString("Weather", (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 4, 1);
    tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);

    if (_state == WeatherState::LOADING) {
        tft.setTextColor(c.textMuted);
        tft.drawString("Loading...", 30, 60, 1);
    } else if (_state == WeatherState::ERROR) {
        tft.setTextColor(c.error);
        tft.drawString("Failed to load.", 20, 50, 1);
        tft.setTextColor(c.textMuted);
        tft.drawString("Check Wi-Fi", 20, 70, 1);
    } else if (_state == WeatherState::LOADED) {
        tft.setTextColor(c.text);
        
        char tempStr[16];
        snprintf(tempStr, sizeof(tempStr), "%.1f C", _temperature);
        tft.drawString("Temp:", 10, 45, 1);
        tft.setTextColor(c.primary);
        tft.drawString(tempStr, 50, 45, 2);
        
        tft.setTextColor(c.text);
        tft.drawString("Wind:", 10, 70, 1);
        char windStr[16];
        snprintf(windStr, sizeof(windStr), "%.1f km/h", _windSpeed);
        tft.drawString(windStr, 50, 70, 1);
        
        char codeStr[16];
        snprintf(codeStr, sizeof(codeStr), "Code: %d", _weatherCode);
        tft.setTextColor(c.textMuted);
        tft.drawString(codeStr, 10, 95, 1);
    }
}

void WeatherApp::handleInput(AppEvent e) {
    if (e == AppEvent::BTN_SELECT_LONG) {
        requestExit = true;
    } else if (e == AppEvent::BTN_SELECT_SHORT) {
        _state = WeatherState::LOADING;
        _needsRedraw = true;
        // The render loop will draw "Loading..." before we block.
        // Wait, fetchWeather() blocks immediately if we call it here. Let's just call it.
        // In a real app we'd do async, but for phase 6 this is fine.
        fetchWeather();
    }
}
