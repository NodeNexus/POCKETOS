#pragma once
// =============================================================================
// SettingsApp.h — System settings application (Phase 1)
//
// Screens: Main → Display → Theme → Wi-Fi (stub) → Storage → About
// Uses the Menu widget for all list navigation.
// =============================================================================

#include "src/apps/App.h"
#include "src/ui/Menu.h"
#include "src/ui/Theme.h"
#include "src/board_config.h"

enum class SettingsScreen : uint8_t {
    MAIN = 0,
    DISPLAY_MENU,
    THEME_SELECT,
    WIFI,
    STORAGE,
    ABOUT
};

class SettingsApp : public App {
public:
    SettingsApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    // Static icon drawing function for the Launcher
    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    // Menu builders
    void buildMain();
    void buildDisplay();
    void buildTheme();
    void buildWifi();
    void buildStorage();

    // Screen renderers
    void renderHeader(TFT_eSPI& tft, const char* title);
    void renderAbout (TFT_eSPI& tft);

    // Input handlers per screen
    void handleMain    (AppEvent e);
    void handleDisplay (AppEvent e);
    void handleTheme   (AppEvent e);
    void handleWifi    (AppEvent e);
    void handleStorage (AppEvent e);
    void handleAbout   (AppEvent e);

    void applyAndSave();

    Menu           _menu;
    SettingsScreen _screen      = SettingsScreen::MAIN;
    bool           _needsRedraw = true;

    static const AppInfo _info;
};
