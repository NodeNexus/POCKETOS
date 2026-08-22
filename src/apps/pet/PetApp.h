#pragma once
// =============================================================================
// PetApp.h — Tamagotchi-style Virtual Pet
// =============================================================================

#include "src/apps/App.h"
#include <ArduinoJson.h>

class PetApp : public App {
public:
    PetApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    void loadState();
    void saveState();
    void renderPet(TFT_eSPI& tft);

    bool           _needsRedraw = true;
    
    // Pet Stats (0-100)
    int            _hunger = 50;
    int            _happiness = 50;
    int            _sleep = 50;
    
    // UI state
    int            _selectedAction = 0; // 0=Feed, 1=Play, 2=Sleep
    uint32_t       _lastUpdateMs = 0;
    
    static const AppInfo _info;
};
