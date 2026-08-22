#pragma once
// =============================================================================
// StatusBar.h — Top 12-pixel status strip
//
// Displays: time (left), Wi-Fi icon + SD icon (right).
// Only redraws when marked dirty — typically once per second.
// =============================================================================

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "src/board_config.h"

class StatusBar {
public:
    static StatusBar& instance();

    void init();
    void update();                    // Refresh cached values (call each loop)
    void render(TFT_eSPI& tft);       // Redraw if dirty
    void markDirty() { _dirty = true; }
    bool isDirty()   const { return _dirty; }

    static constexpr int16_t HEIGHT = STATUS_BAR_HEIGHT;

private:
    StatusBar() = default;

    bool     _dirty          = true;
    uint32_t _lastUpdateMs   = 0;
    char     _timeStr[8]     = "--:--";
    bool     _wifiConnected  = false;
    int8_t   _wifiStrength   = 0;
    bool     _sdPresent      = false;
};
