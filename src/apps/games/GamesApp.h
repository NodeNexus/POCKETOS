#pragma once
// =============================================================================
// GamesApp.h — Mini-games
// =============================================================================

#include "src/apps/App.h"
#include "src/ui/Menu.h"

enum class GameState {
    MENU,
    SNAKE,
    PONG,
    BREAKOUT,
    GAME_2048
};

class GamesApp : public App {
public:
    GamesApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    void initSnake();
    void updateSnake(uint32_t deltaMs);
    void renderSnake(TFT_eSPI& tft);

    GameState      _state = GameState::MENU;
    Menu           _menu;
    bool           _needsRedraw = true;
    
    // Snake state
    uint8_t        _snakeX[64];
    uint8_t        _snakeY[64];
    uint8_t        _snakeLen = 3;
    int8_t         _snakeDirX = 1;
    int8_t         _snakeDirY = 0;
    uint8_t        _appleX = 0;
    uint8_t        _appleY = 0;
    uint32_t       _snakeTimer = 0;
    bool           _snakeDead = false;
    
    static const AppInfo _info;
};
