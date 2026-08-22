#pragma once
// =============================================================================
// Widgets.h — Reusable stateless UI drawing primitives
//
// All functions accept a TFT_eSPI reference and draw directly.
// No state is stored; callers are responsible for positioning.
// =============================================================================

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "src/ui/Theme.h"
#include "src/board_config.h"

namespace Widgets {

    // Horizontal progress bar with rounded caps
    void drawProgressBar(TFT_eSPI& tft,
                         int16_t x, int16_t y, int16_t w, int16_t h,
                         float progress,           // 0.0 – 1.0
                         uint16_t fillColor,
                         uint16_t bgColor,
                         uint16_t borderColor);

    // On/Off toggle switch (24×12 px)
    void drawToggle(TFT_eSPI& tft, int16_t x, int16_t y,
                    bool state, uint16_t onColor, uint16_t offColor);

    // Vertical scrollbar indicator
    void drawScrollbar(TFT_eSPI& tft,
                       int16_t x, int16_t y, int16_t h,
                       uint8_t total, uint8_t visible, uint8_t offset,
                       uint16_t trackColor, uint16_t thumbColor);

    // 4-bar signal-strength indicator (x, y = top-left of icon area)
    void drawSignalBars(TFT_eSPI& tft, int16_t x, int16_t y,
                        uint8_t strength,   // 0-4
                        uint16_t activeColor, uint16_t inactiveColor);

    // Compact WiFi icon (~14×10 px, centred at cx,cy)
    void drawWifiIcon(TFT_eSPI& tft, int16_t cx, int16_t cy,
                      bool connected, int8_t strength,  // 0-4
                      uint16_t color);

    // Compact SD card icon (~8×10 px at x,y)
    void drawSDIcon(TFT_eSPI& tft, int16_t x, int16_t y,
                    bool present, uint16_t color);

    // Battery icon (~16×8 px at x,y)
    void drawBatteryIcon(TFT_eSPI& tft, int16_t x, int16_t y,
                         uint8_t percent, bool charging, uint16_t color);

    // Green tick mark centred at cx,cy
    void drawCheckmark(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

    // Red X mark centred at cx,cy
    void drawCrossMark(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

    // Full-width horizontal divider
    void drawDivider(TFT_eSPI& tft, int16_t y, uint16_t color);

} // namespace Widgets
