#pragma once
// =============================================================================
// MonitorApp.h — Raspberry Pi Monitor
// =============================================================================

#include "src/apps/App.h"
#include <ArduinoJson.h>

enum class MonitorState {
    LOADING,
    LOADED,
    ERROR
};

class MonitorApp : public App {
public:
    MonitorApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    void fetchStatus();

    MonitorState   _state = MonitorState::LOADING;
    bool           _needsRedraw = true;
    
    // Status data
    float          _cpuLoad = 0.0f;
    float          _ramUsage = 0.0f;
    float          _temperature = 0.0f;
    
    static const AppInfo _info;
};
