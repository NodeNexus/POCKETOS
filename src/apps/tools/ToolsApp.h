#pragma once
// =============================================================================
// ToolsApp.h — Utilities
// =============================================================================

#include "src/apps/App.h"
#include "src/ui/Menu.h"

enum class ToolState {
    MENU,
    CLOCK,
    STOPWATCH
};

class ToolsApp : public App {
public:
    ToolsApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    ToolState      _state = ToolState::MENU;
    Menu           _menu;
    bool           _needsRedraw = true;
    
    // Stopwatch state
    bool           _swRunning = false;
    uint32_t       _swStartMs = 0;
    uint32_t       _swElapsedMs = 0;
    
    static const AppInfo _info;
};
