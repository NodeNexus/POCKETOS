#include "src/core/DisplayManager.h"
#include "src/debug.h"
#include "src/board_config.h"
#include <math.h>
#if __has_include(<esp_arduino_version.h>)
#include <esp_arduino_version.h>
#endif

DisplayManager& DisplayManager::instance() {
    static DisplayManager inst;
    return inst;
}

bool DisplayManager::init() {
    LOG_INFO("DisplayManager: init TFT_eSPI (ST7735S 128x128)");

    _tft.init();
    _tft.setRotation(DISPLAY_ROTATION);
    _tft.fillScreen(TFT_BLACK);

    // Configure LEDC backlight PWM
#if defined(ESP_ARDUINO_VERSION) && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    ledcAttach(PIN_BACKLIGHT, BL_PWM_FREQ, BL_PWM_RESOLUTION);
#else
    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ, BL_PWM_RESOLUTION);
    ledcAttachPin(PIN_BACKLIGHT, BL_PWM_CHANNEL);
#endif
    applyBrightness(0);   // Start dark; boot animation fades in

    _initialized = true;
    LOG_INFO("DisplayManager: OK (%dx%d, rotation=%d)",
             SCREEN_WIDTH, SCREEN_HEIGHT, DISPLAY_ROTATION);
    return true;
}

void DisplayManager::update() {
    if (!_fading) return;

    uint32_t elapsed = millis() - _fadeStartMs;
    if (elapsed >= _fadeDurationMs) {
        applyBrightness(_fadeTarget);
        _brightness = _fadeTarget;
        _fading     = false;
        return;
    }

    // Linear interpolation between start and target brightness
    float    t       = (float)elapsed / (float)_fadeDurationMs;
    uint8_t  current = (uint8_t)(_fadeStart + t * ((int16_t)_fadeTarget - _fadeStart));
    applyBrightness(current);
}

void DisplayManager::setBrightness(uint8_t brightness) {
    _fading     = false;
    _brightness = brightness;
    applyBrightness(brightness);
}

void DisplayManager::fadeToBrightness(uint8_t target, uint16_t durationMs) {
    if (_fading && _fadeTarget == target) return;
    _fadeStart      = _brightness;
    _fadeTarget     = target;
    _fadeStartMs    = millis();
    _fadeDurationMs = durationMs;
    _fading         = true;
}

void DisplayManager::setBacklightOn(bool on) {
    _fading = false;
    applyBrightness(on ? _brightness : 0);
}

void DisplayManager::applyBrightness(uint8_t value) {
#if defined(ESP_ARDUINO_VERSION) && ESP_ARDUINO_VERSION >= ESP_ARDUINO_VERSION_VAL(3, 0, 0)
    ledcWrite(PIN_BACKLIGHT, value);
#else
    ledcWrite(BL_PWM_CHANNEL, value);
#endif
}

void DisplayManager::clear(uint16_t color) {
    _tft.fillScreen(color);
}

void DisplayManager::drawCenteredText(const char* text, int16_t y,
                                      uint16_t color, uint8_t font) {
    _tft.setTextColor(color, 0x0000);  // solid background prevents fringing
    int16_t w = _tft.textWidth(text, font);
    int16_t x = max(0, (SCREEN_WIDTH - w) / 2);
    _tft.drawString(text, x, y, font);
}

void DisplayManager::drawSpinner(int16_t cx, int16_t cy, uint8_t radius,
                                 uint16_t color, uint8_t frame) {
    const uint8_t N = 8;
    for (uint8_t i = 0; i < N; i++) {
        float    angle = (float)i * (2.0f * (float)M_PI / N);
        int16_t  x     = cx + (int16_t)(cosf(angle) * radius);
        int16_t  y     = cy + (int16_t)(sinf(angle) * radius);
        // Active dot = full brightness; others = quarter brightness
        uint16_t c     = (i == (frame % N))
                         ? color
                         : (((color >> 11) >> 1) << 11)
                           | (((color >> 5 & 0x3F) >> 1) << 5)
                           | ((color & 0x1F) >> 1);
        _tft.fillCircle(x, y, 1, c);
    }
}
