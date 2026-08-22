#pragma once
// =============================================================================
// Launcher.h — 3×3 icon-grid home screen
//
// Linear navigation: UP decrements index, DOWN increments, SELECT launches.
// Supports up to LAUNCHER_MAX_APPS apps with page scrolling for overflow.
// =============================================================================

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "src/apps/App.h"
#include "src/ui/Theme.h"
#include "src/board_config.h"

static constexpr uint8_t LAUNCHER_COLS     = 3;
static constexpr uint8_t LAUNCHER_ROWS     = 3;
static constexpr uint8_t LAUNCHER_MAX_APPS = 12;

// Grid geometry — fits into 128×116 content area
static constexpr int16_t CELL_W   = 42;    // 3 × 42 = 126  (2 px margin)
static constexpr int16_t CELL_H   = 38;    // 3 × 38 = 114  (2 px margin)
static constexpr int16_t GRID_TOP = STATUS_BAR_HEIGHT + 2;

class Launcher : public App {
public:
    Launcher();

    bool    registerApp(App* app);
    App*    getApp(uint8_t index) const;
    uint8_t getAppCount()         const { return _appCount; }

    // App interface
    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override {}

private:
    void     drawGrid(TFT_eSPI& tft);
    void     drawCell(TFT_eSPI& tft, uint8_t col, uint8_t row,
                      uint8_t appIdx, bool selected);
    int16_t  cellCX(uint8_t col) const { return col * CELL_W + CELL_W / 2; }
    int16_t  cellCY(uint8_t row) const { return GRID_TOP + row * CELL_H + CELL_H / 2 - 6; }

    App*     _apps[LAUNCHER_MAX_APPS];
    uint8_t  _appCount   = 0;
    uint8_t  _selected   = 0;
    uint8_t  _pageStart  = 0;    // Index of first app on current page
    bool     _needsRedraw = true;

    static const AppInfo _info;
};
