#pragma once
// =============================================================================
// PlaceholderApp.h — Generic stub for apps not yet implemented
//
// Displays the app name, its target phase, and a "COMING SOON" message.
// Used for all apps except Settings in Phase 1.
// =============================================================================

#include "src/apps/App.h"
#include "src/ui/Theme.h"
#include "src/board_config.h"

class PlaceholderApp : public App {
public:
    PlaceholderApp(uint8_t id, const char* name, const char* phase,
                   void (*iconDraw)(TFT_eSPI&, int16_t, int16_t, uint16_t) = nullptr);

    const AppInfo& getInfo()  const override { return _info; }
    void onEnter()                  override { _rendered = false; }
    void render(TFT_eSPI& tft)      override;
    void handleInput(AppEvent event) override;

private:
    AppInfo _info;
    char    _phase[20];
    bool    _rendered = false;
};
