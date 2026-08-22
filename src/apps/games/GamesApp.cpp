#include "src/apps/games/GamesApp.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include <stdlib.h>

void GamesApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.drawRect(cx - 8, cy - 6, 16, 12, color);
    tft.fillRect(cx - 6, cy - 4, 4, 4, color);
    tft.drawFastHLine(cx + 2, cy - 2, 4, color);
    tft.drawFastVLine(cx + 4, cy - 4, 4, color);
}

const AppInfo GamesApp::_info = { 1, "Games", GamesApp::drawIcon };

GamesApp::GamesApp() {}

void GamesApp::onCreate() {
    LOG_INFO("GamesApp: onCreate");
}

void GamesApp::onEnter() {
    _state = GameState::MENU;
    _needsRedraw = true;
    _menu.clear();
    _menu.addItem("Snake");
    _menu.addItem("Pong");
    _menu.addItem("Breakout");
    _menu.addItem("2048");
}

void GamesApp::onExit() {
}

void GamesApp::initSnake() {
    _snakeLen = 3;
    _snakeX[0] = 10; _snakeY[0] = 10;
    _snakeX[1] = 9;  _snakeY[1] = 10;
    _snakeX[2] = 8;  _snakeY[2] = 10;
    _snakeDirX = 1;  _snakeDirY = 0;
    _snakeDead = false;
    _appleX = 15; _appleY = 10;
    _snakeTimer = 0;
}

void GamesApp::updateSnake(uint32_t deltaMs) {
    if (_snakeDead) return;
    
    _snakeTimer += deltaMs;
    if (_snakeTimer > 200) {
        _snakeTimer = 0;
        
        // Move body
        for (int i = _snakeLen - 1; i > 0; i--) {
            _snakeX[i] = _snakeX[i-1];
            _snakeY[i] = _snakeY[i-1];
        }
        
        // Move head
        _snakeX[0] += _snakeDirX;
        _snakeY[0] += _snakeDirY;
        
        // Check apple
        if (_snakeX[0] == _appleX && _snakeY[0] == _appleY) {
            if (_snakeLen < 64) _snakeLen++;
            _appleX = rand() % 20;
            _appleY = 2 + rand() % 16;
        }
        
        // Check walls & self
        if (_snakeX[0] >= 21 || _snakeY[0] >= 20 || _snakeY[0] < 2) {
            _snakeDead = true;
        }
        for (int i = 1; i < _snakeLen; i++) {
            if (_snakeX[0] == _snakeX[i] && _snakeY[0] == _snakeY[i]) {
                _snakeDead = true;
            }
        }
        _needsRedraw = true;
    }
}

void GamesApp::renderSnake(TFT_eSPI& tft) {
    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);
    
    if (_snakeDead) {
        tft.setTextColor(c.error);
        int16_t tw = tft.textWidth("GAME OVER", 2);
        tft.drawString("GAME OVER", (SCREEN_WIDTH - tw)/2, 60, 2);
        return;
    }
    
    // Draw apple
    tft.fillRect(_appleX * 6, _appleY * 6, 5, 5, c.error);
    
    // Draw snake
    for (int i = 0; i < _snakeLen; i++) {
        tft.fillRect(_snakeX[i] * 6, _snakeY[i] * 6, 5, 5, i == 0 ? c.primary : c.text);
    }
}

void GamesApp::update(uint32_t deltaMs) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
    
    if (_state == GameState::SNAKE) {
        updateSnake(deltaMs);
    }
}

void GamesApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    
    if (_state == GameState::MENU) {
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
        tft.setTextColor(c.primary);
        int16_t tw = tft.textWidth("Games", 1);
        tft.drawString("Games", (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 4, 1);
        tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);
        _menu.render(tft, STATUS_BAR_HEIGHT + 19, CONTENT_HEIGHT - 19);
    } else if (_state == GameState::SNAKE) {
        renderSnake(tft);
    } else {
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);
        tft.setTextColor(c.textMuted);
        const char* msg = "Coming Soon";
        int16_t mw = tft.textWidth(msg, 2);
        tft.drawString(msg, (SCREEN_WIDTH - mw) / 2, 55, 2);
        tft.setTextColor(c.textDisabled);
        const char* hint = "SEL = Back";
        tft.drawString(hint, (SCREEN_WIDTH - tft.textWidth(hint, 1)) / 2, 80, 1);
    }
}

void GamesApp::handleInput(AppEvent e) {
    if (_state == GameState::MENU) {
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
                if (_menu.getSelected() == 0) {
                    _state = GameState::SNAKE;
                    initSnake();
                } else if (_menu.getSelected() == 1) _state = GameState::PONG;
                else if (_menu.getSelected() == 2) _state = GameState::BREAKOUT;
                else if (_menu.getSelected() == 3) _state = GameState::GAME_2048;
                _needsRedraw = true;
                break;
            case AppEvent::BTN_SELECT_LONG:
                requestExit = true;
                break;
            default: break;
        }
    } else if (_state == GameState::SNAKE) {
        if (e == AppEvent::BTN_UP_SHORT || e == AppEvent::BTN_UP_HELD) {
            if (_snakeDirY == 0) { _snakeDirX = 0; _snakeDirY = -1; }
        } else if (e == AppEvent::BTN_DOWN_SHORT || e == AppEvent::BTN_DOWN_HELD) {
            if (_snakeDirY == 0) { _snakeDirX = 0; _snakeDirY = 1; }
        } else if (e == AppEvent::BTN_SELECT_SHORT) {
            if (_snakeDirX == 0) { _snakeDirX = 1; _snakeDirY = 0; } // simplified turn
            else if (_snakeDead) initSnake();
        } else if (e == AppEvent::BTN_SELECT_LONG) {
            _state = GameState::MENU;
            _needsRedraw = true;
        }
    } else {
        if (e == AppEvent::BTN_SELECT_LONG || e == AppEvent::BTN_SELECT_SHORT) {
            _state = GameState::MENU;
            _needsRedraw = true;
        }
    }
}
