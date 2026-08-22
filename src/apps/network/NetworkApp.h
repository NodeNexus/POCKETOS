#pragma once
// =============================================================================
// NetworkApp.h — Network Scanner and Status
// =============================================================================

#include "src/apps/App.h"
#include "src/ui/Menu.h"

class NetworkApp : public App {
public:
    NetworkApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    void rebuildMenu();

    Menu           _menu;
    bool           _needsRedraw = true;
    bool           _scanning = false;
    
    static const AppInfo _info;
};
