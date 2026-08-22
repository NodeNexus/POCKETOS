#include "src/apps/pet/PetApp.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include "src/core/StorageManager.h"
#include <ArduinoJson.h>

void PetApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.fillCircle(cx, cy + 2, 6, color);
    const ThemeColors& c = Theme::instance().colors();
    tft.fillCircle(cx - 2, cy + 1, 1, c.background);
    tft.fillCircle(cx + 2, cy + 1, 1, c.background);
    tft.drawFastHLine(cx - 1, cy + 4, 3, c.background);
}

const AppInfo PetApp::_info = { 7, "Pet", PetApp::drawIcon };

PetApp::PetApp() {}

void PetApp::onCreate() {
    LOG_INFO("PetApp: onCreate");
}

void PetApp::onEnter() {
    _needsRedraw = true;
    _selectedAction = 0;
    loadState();
}

void PetApp::onExit() {
    saveState();
}

void PetApp::loadState() {
    String payload;
    if (StorageManager::instance().readFile("/PocketOS/saves/pet.json", payload)) {
        JsonDocument doc;
        if (!deserializeJson(doc, payload)) {
            _hunger = doc["hunger"] | 50;
            _happiness = doc["happiness"] | 50;
            _sleep = doc["sleep"] | 50;
        }
    }
}

void PetApp::saveState() {
    JsonDocument doc;
    doc["hunger"] = _hunger;
    doc["happiness"] = _happiness;
    doc["sleep"] = _sleep;
    
    String payload;
    serializeJson(doc, payload);
    StorageManager::instance().writeFile("/PocketOS/saves/pet.json", payload.c_str(), false);
}

void PetApp::update(uint32_t deltaMs) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
    
    _lastUpdateMs += deltaMs;
    // Degrade stats slowly over time (simulate every 10 seconds while app is open)
    if (_lastUpdateMs > 10000) {
        _lastUpdateMs = 0;
        if (_hunger > 0) _hunger--;
        if (_happiness > 0) _happiness--;
        if (_sleep > 0) _sleep--;
        _needsRedraw = true;
    }
}

void PetApp::renderPet(TFT_eSPI& tft) {
    const ThemeColors& c = Theme::instance().colors();
    int cx = SCREEN_WIDTH / 2;
    int cy = 60;
    
    // Draw blob body
    tft.fillCircle(cx, cy, 20, c.primary);
    
    // Draw eyes based on stats
    uint16_t eyeColor = c.background;
    if (_sleep < 20) {
        // Sleepy eyes
        tft.drawFastHLine(cx - 8, cy - 2, 6, eyeColor);
        tft.drawFastHLine(cx + 2, cy - 2, 6, eyeColor);
    } else {
        // Normal eyes
        tft.fillCircle(cx - 6, cy - 2, 3, eyeColor);
        tft.fillCircle(cx + 6, cy - 2, 3, eyeColor);
    }
    
    // Draw mouth based on happiness
    if (_happiness > 60) {
        // Happy smile (V shape)
        tft.drawLine(cx - 4, cy + 4, cx, cy + 8, eyeColor);
        tft.drawLine(cx, cy + 8, cx + 4, cy + 4, eyeColor);
    } else if (_happiness < 30) {
        // Sad frown (inverted V)
        tft.drawLine(cx - 4, cy + 8, cx, cy + 4, eyeColor);
        tft.drawLine(cx, cy + 4, cx + 4, cy + 8, eyeColor);
    } else {
        // Neutral line
        tft.drawFastHLine(cx - 4, cy + 6, 8, eyeColor);
    }
}

void PetApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    renderPet(tft);
    
    // Draw stats
    tft.setTextColor(c.text);
    char buf[32];
    snprintf(buf, sizeof(buf), "H:%d J:%d S:%d", _hunger, _happiness, _sleep);
    int16_t tw = tft.textWidth(buf, 1);
    tft.drawString(buf, (SCREEN_WIDTH - tw) / 2, 90, 1);
    
    // Draw Action Menu
    const char* actions[] = {"Feed", "Play", "Sleep"};
    int actionW = SCREEN_WIDTH / 3;
    for (int i = 0; i < 3; i++) {
        if (i == _selectedAction) {
            tft.fillRect(i * actionW, 110, actionW, 18, c.primary);
            tft.setTextColor(c.background);
        } else {
            tft.fillRect(i * actionW, 110, actionW, 18, c.surface);
            tft.setTextColor(c.text);
        }
        int16_t aw = tft.textWidth(actions[i], 1);
        tft.drawString(actions[i], i * actionW + (actionW - aw) / 2, 115, 1);
    }
}

void PetApp::handleInput(AppEvent e) {
    switch (e) {
        case AppEvent::BTN_UP_SHORT:
        case AppEvent::BTN_UP_HELD:
            _selectedAction = (_selectedAction + 2) % 3; // equivalent to -1 mod 3
            _needsRedraw = true;
            break;
        case AppEvent::BTN_DOWN_SHORT:
        case AppEvent::BTN_DOWN_HELD:
            _selectedAction = (_selectedAction + 1) % 3;
            _needsRedraw = true;
            break;
        case AppEvent::BTN_SELECT_SHORT:
            if (_selectedAction == 0) {
                _hunger = min(100, _hunger + 20);
            } else if (_selectedAction == 1) {
                _happiness = min(100, _happiness + 20);
                _hunger = max(0, _hunger - 10);
                _sleep = max(0, _sleep - 10);
            } else if (_selectedAction == 2) {
                _sleep = 100;
                _hunger = max(0, _hunger - 20);
            }
            saveState();
            _needsRedraw = true;
            break;
        case AppEvent::BTN_SELECT_LONG:
            requestExit = true;
            break;
        default: break;
    }
}
