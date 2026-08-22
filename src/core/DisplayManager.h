#pragma once
// =============================================================================
// DisplayManager.h — TFT display initialisation and backlight control
//
// Wraps TFT_eSPI. Exposes tft() so drawing code can use TFT_eSPI directly
// without being coupled to the manager for every primitive.
//
// Backlight is PWM-controlled via LEDC. Non-blocking brightness fades are
// handled in update(), called each main-loop iteration.
// =============================================================================

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "src/board_config.h"

class DisplayManager {
public:
    static DisplayManager& instance();

    // Must be called in setup() before any drawing
    bool init();

    // Call every loop() to advance non-blocking brightness fades
    void update();

    // Direct TFT access for drawing operations
    TFT_eSPI& tft() { return _tft; }

    // -----------------------------------------------------------------------
    // Backlight control
    // -----------------------------------------------------------------------
    void    setBrightness(uint8_t brightness);          // 0-255, immediate
    void    fadeToBrightness(uint8_t target,
                             uint16_t durationMs);      // non-blocking
    void    setBacklightOn(bool on);                    // immediate on/off

    uint8_t getBrightness() const { return _brightness; }

    // -----------------------------------------------------------------------
    // Convenience drawing helpers
    // -----------------------------------------------------------------------
    // Fill the entire screen with one colour
    void clear(uint16_t color = 0x0000);

    // Draw text centred horizontally at a given y coordinate
    void drawCenteredText(const char* text, int16_t y,
                          uint16_t color, uint8_t font = 2);

    // Animated spinner: draw 8 dots around a circle; frame cycles 0-7
    void drawSpinner(int16_t cx, int16_t cy, uint8_t radius,
                     uint16_t color, uint8_t frame);

    bool isInitialized() const { return _initialized; }

private:
    DisplayManager() : _tft() {}

    void applyBrightness(uint8_t value);

    TFT_eSPI _tft;
    bool     _initialized     = false;
    uint8_t  _brightness      = BL_DEFAULT_BRIGHTNESS;

    // Non-blocking fade state
    bool     _fading          = false;
    uint8_t  _fadeStart       = 0;
    uint8_t  _fadeTarget      = 0;
    uint32_t _fadeStartMs     = 0;
    uint16_t _fadeDurationMs  = 0;
};
