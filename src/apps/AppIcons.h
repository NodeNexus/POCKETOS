#pragma once
// =============================================================================
// AppIcons.h — Icon drawing functions for each launcher application
//
// Each function draws a ~20×20 icon centred at (cx, cy) using TFT_eSPI
// primitives only — no bitmap files required.
// All functions follow the signature required by AppInfo::drawIcon.
// =============================================================================

#include <TFT_eSPI.h>
#include <Arduino.h>
#include <math.h>

// ---------------------------------------------------------------------------
// Games — game controller outline
// ---------------------------------------------------------------------------
inline void iconGames(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    tft.drawRoundRect(cx - 9, cy - 5, 18, 11, 4, c);  // Body
    // D-pad (cross) on left
    tft.drawFastHLine(cx - 8, cy - 1, 5, c);
    tft.drawFastVLine(cx - 6, cy - 3, 5, c);
    // A/B buttons on right
    tft.fillCircle(cx + 5, cy - 2, 2, c);
    tft.fillCircle(cx + 8, cy + 1, 2, c);
    // Shoulder bumpers
    tft.drawFastHLine(cx - 7, cy - 5, 3, c);
    tft.drawFastHLine(cx + 4, cy - 5, 3, c);
}

// ---------------------------------------------------------------------------
// Network — WiFi / antenna waves
// ---------------------------------------------------------------------------
inline void iconNetwork(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    tft.fillCircle(cx, cy + 6, 2, c);
    // Three concentric upper arcs using drawArc
    tft.drawArc(cx, cy + 7, 5,  3,  310, 50, c, 0x0000);
    tft.drawArc(cx, cy + 7, 9,  7,  310, 50, c, 0x0000);
    tft.drawArc(cx, cy + 7, 13, 11, 310, 50, c, 0x0000);
}

// ---------------------------------------------------------------------------
// Monitor — display with bar graph inside
// ---------------------------------------------------------------------------
inline void iconMonitor(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    tft.drawRoundRect(cx - 9, cy - 6, 18, 13, 2, c);   // Screen outline
    tft.drawFastHLine(cx - 3, cy + 7, 6, c);           // Stand
    tft.drawFastHLine(cx - 5, cy + 8, 10, c);          // Base
    // Bar graph inside screen
    tft.fillRect(cx - 6, cy + 1, 2, 4, c);
    tft.fillRect(cx - 3, cy - 1, 2, 6, c);
    tft.fillRect(cx,     cy + 2, 2, 3, c);
    tft.fillRect(cx + 3, cy - 2, 2, 7, c);
}

// ---------------------------------------------------------------------------
// Files — folder shape
// ---------------------------------------------------------------------------
inline void iconFiles(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    // Folder tab
    tft.fillRect(cx - 8, cy - 5, 7, 3, c);
    // Folder body
    tft.drawRect(cx - 8, cy - 3, 16, 10, c);
    // Lines inside (document lines)
    tft.drawFastHLine(cx - 5, cy + 1, 10, c);
    tft.drawFastHLine(cx - 5, cy + 4, 7,  c);
}

// ---------------------------------------------------------------------------
// Tools — wrench silhouette
// ---------------------------------------------------------------------------
inline void iconTools(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    // Diagonal shaft
    tft.drawLine(cx - 7, cy + 7, cx + 5, cy - 5, c);
    tft.drawLine(cx - 6, cy + 7, cx + 6, cy - 5, c);
    // Handle circle
    tft.drawCircle(cx - 6, cy + 6, 3, c);
    // Head circle
    tft.drawCircle(cx + 5, cy - 5, 3, c);
    tft.fillCircle(cx + 5, cy - 5, 2, c);
}

// ---------------------------------------------------------------------------
// Weather — cloud with sun rays
// ---------------------------------------------------------------------------
inline void iconWeather(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    // Cloud
    tft.fillCircle(cx - 3, cy + 1, 5, c);
    tft.fillCircle(cx + 3, cy + 2, 4, c);
    tft.fillRect  (cx - 3, cy + 1, 7, 5, c);
    // Sun (small circle top-right)
    tft.drawCircle(cx + 6, cy - 5, 3, c);
    // Sun rays
    tft.drawLine(cx + 6, cy - 9, cx + 6, cy - 10, c);
    tft.drawLine(cx + 9, cy - 7, cx + 10, cy - 8, c);
    tft.drawLine(cx + 10, cy - 4, cx + 11, cy - 4, c);
}

// ---------------------------------------------------------------------------
// Pet — friendly face (circle + eyes + smile)
// ---------------------------------------------------------------------------
inline void iconPet(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    tft.drawCircle(cx, cy, 8, c);      // Head
    tft.fillCircle(cx - 3, cy - 2, 1, c);  // Left eye
    tft.fillCircle(cx + 3, cy - 2, 1, c);  // Right eye
    // Smile arc (simplified as two angled lines)
    tft.drawLine(cx - 3, cy + 2, cx,     cy + 4, c);
    tft.drawLine(cx,     cy + 4, cx + 3, cy + 2, c);
    // Ears
    tft.drawLine(cx - 6, cy - 7, cx - 4, cy - 9, c);
    tft.drawLine(cx + 6, cy - 7, cx + 4, cy - 9, c);
}

// ---------------------------------------------------------------------------
// Photos — camera body with lens
// ---------------------------------------------------------------------------
inline void iconPhotos(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    tft.drawRoundRect(cx - 8, cy - 4, 16, 11, 2, c);  // Body
    tft.drawRect(cx - 3, cy - 6, 6, 3, c);            // Viewfinder bump
    tft.drawCircle(cx, cy + 1, 4, c);                 // Lens outer
    tft.drawCircle(cx, cy + 1, 2, c);                 // Lens inner
}

// ---------------------------------------------------------------------------
// Audio — musical note
// ---------------------------------------------------------------------------
inline void iconAudio(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    // Note head
    tft.fillCircle(cx - 2, cy + 5, 3, c);
    tft.fillCircle(cx + 5, cy + 3, 3, c);
    // Note stem
    tft.drawFastVLine(cx + 1, cy - 6, 12, c);
    tft.drawFastVLine(cx + 8, cy - 8, 12, c);
    // Beam connecting stems
    tft.drawFastHLine(cx + 1, cy - 6, 8, c);
}

// ---------------------------------------------------------------------------
// Settings — gear shape
// ---------------------------------------------------------------------------
inline void iconSettings(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t c) {
    tft.drawCircle(cx, cy, 5, c);     // Outer ring
    tft.fillCircle(cx, cy, 3, c);     // Hub
    // 8 teeth around the ring
    for (int a = 0; a < 360; a += 45) {
        float rad = (float)a * (float)M_PI / 180.0f;
        int16_t x1 = cx + (int16_t)(cosf(rad) * 6);
        int16_t y1 = cy + (int16_t)(sinf(rad) * 6);
        int16_t x2 = cx + (int16_t)(cosf(rad) * 9);
        int16_t y2 = cy + (int16_t)(sinf(rad) * 9);
        tft.drawLine(x1, y1, x2, y2, c);
    }
}
