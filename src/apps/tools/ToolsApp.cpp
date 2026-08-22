#include "src/apps/tools/ToolsApp.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include "src/core/TimeManager.h"
#include <stdio.h>

void ToolsApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.drawCircle(cx, cy, 10, color);
    tft.drawLine(cx, cy, cx, cy - 6, color);
    tft.drawLine(cx, cy, cx + 4, cy + 4, color);
}

const AppInfo ToolsApp::_info = { 5, "Tools", ToolsApp::drawIcon };

ToolsApp::ToolsApp() {}

void ToolsApp::onCreate() {
    LOG_INFO("ToolsApp: onCreate");
}

void ToolsApp::onEnter() {
    _state = ToolState::MENU;
    _needsRedraw = true;
    _menu.clear();
    _menu.addItem("Clock");
    _menu.addItem("Stopwatch");
}

void ToolsApp::onExit() {
}

void ToolsApp::update(uint32_t /*deltaMs*/) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
    
    if (_state == ToolState::STOPWATCH && _swRunning) {
        _needsRedraw = true; // Constantly redraw stopwatch
    }
    if (_state == ToolState::CLOCK) {
        // Redraw every second to update clock, TimeManager already triggers status bar dirty on minute change, but we want seconds
        static uint32_t lastClockMs = 0;
        if (millis() - lastClockMs > 1000) {
            lastClockMs = millis();
            _needsRedraw = true;
        }
    }
}

void ToolsApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    
    if (_state == ToolState::MENU) {
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
        tft.setTextColor(c.primary);
        int16_t tw = tft.textWidth("Tools", 1);
        tft.drawString("Tools", (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 4, 1);
        tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);
        _menu.render(tft, STATUS_BAR_HEIGHT + 19, CONTENT_HEIGHT - 19);
    } else if (_state == ToolState::STOPWATCH) {
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);
        tft.setTextColor(c.text);
        
        uint32_t currentElapsed = _swElapsedMs;
        if (_swRunning) {
            currentElapsed += (millis() - _swStartMs);
        }
        
        uint32_t ms = currentElapsed % 1000;
        uint32_t s = (currentElapsed / 1000) % 60;
        uint32_t m = (currentElapsed / 60000) % 60;
        
        char buf[32];
        snprintf(buf, sizeof(buf), "%02u:%02u.%03u", (unsigned)m, (unsigned)s, (unsigned)ms);
        int16_t tw = tft.textWidth(buf, 4);
        tft.drawString(buf, (SCREEN_WIDTH - tw) / 2, 60, 4);
        
        tft.setTextColor(c.textMuted);
        tft.drawString("SELECT: Start/Stop", 10, 100, 1);
        tft.drawString("LONG: Reset/Back", 10, 110, 1);
    } else if (_state == ToolState::CLOCK) {
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

        // Header
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
        tft.setTextColor(c.primary);
        int16_t thw = tft.textWidth("Clock", 1);
        tft.drawString("Clock", (SCREEN_WIDTH - thw) / 2, STATUS_BAR_HEIGHT + 4, 1);
        tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);

        // Time display — uses TimeManager which falls back to uptime if NTP not synced
        char timeBuf[16];
        TimeManager::instance().getTimeString(timeBuf, sizeof(timeBuf));
        tft.setTextColor(c.primary);
        int16_t tw = tft.textWidth(timeBuf, 4);
        tft.drawString(timeBuf, (SCREEN_WIDTH - tw) / 2, 50, 4);

        // Date display
        char dateBuf[16];
        TimeManager::instance().getDateString(dateBuf, sizeof(dateBuf));
        tft.setTextColor(c.textMuted);
        int16_t dw = tft.textWidth(dateBuf, 2);
        tft.drawString(dateBuf, (SCREEN_WIDTH - dw) / 2, 80, 2);

        // Synced indicator
        tft.setTextColor(c.textDisabled);
        const char* syncLabel = TimeManager::instance().isSynced() ? "NTP Synced" : "Not Synced";
        tft.drawString(syncLabel, (SCREEN_WIDTH - tft.textWidth(syncLabel, 1)) / 2, 105, 1);
    }
}

void ToolsApp::handleInput(AppEvent e) {
    if (_state == ToolState::MENU) {
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
            case AppEvent::BTN_SELECT_SHORT:
                if (_menu.getSelected() == 0) _state = ToolState::CLOCK;
                if (_menu.getSelected() == 1) _state = ToolState::STOPWATCH;
                _needsRedraw = true;
                break;
            case AppEvent::BTN_SELECT_LONG:
                requestExit = true;
                break;
            default: break;
        }
    } else if (_state == ToolState::STOPWATCH) {
        if (e == AppEvent::BTN_SELECT_SHORT) {
            if (_swRunning) {
                _swElapsedMs += (millis() - _swStartMs);
                _swRunning = false;
            } else {
                _swStartMs = millis();
                _swRunning = true;
            }
            _needsRedraw = true;
        } else if (e == AppEvent::BTN_SELECT_LONG) {
            if (_swElapsedMs > 0 || _swRunning) {
                _swRunning = false;
                _swElapsedMs = 0;
                _needsRedraw = true;
            } else {
                _state = ToolState::MENU;
                _needsRedraw = true;
            }
        }
    } else if (_state == ToolState::CLOCK) {
        if (e == AppEvent::BTN_SELECT_LONG || e == AppEvent::BTN_SELECT_SHORT) {
            _state = ToolState::MENU;
            _needsRedraw = true;
        }
    }
}
