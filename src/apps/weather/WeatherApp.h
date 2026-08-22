#pragma once
// =============================================================================
// WeatherApp.h — Connected weather widget
// =============================================================================

#include "src/apps/App.h"
#include <ArduinoJson.h>

enum class WeatherState {
    LOADING,
    LOADED,
    ERROR
};

class WeatherApp : public App {
public:
    WeatherApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    void fetchWeather();

    WeatherState   _state = WeatherState::LOADING;
    bool           _needsRedraw = true;
    
    // Weather data
    float          _temperature = 0.0f;
    float          _windSpeed = 0.0f;
    int            _weatherCode = 0;
    
    static const AppInfo _info;
};
