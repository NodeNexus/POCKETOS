#include "src/ui/Widgets.h"
#include <math.h>

namespace Widgets {

void drawProgressBar(TFT_eSPI& tft,
                     int16_t x, int16_t y, int16_t w, int16_t h,
                     float progress,
                     uint16_t fillColor, uint16_t bgColor, uint16_t borderColor) {
    progress = constrain(progress, 0.0f, 1.0f);
    int16_t r = h / 2;
    tft.fillRoundRect(x, y, w, h, r, bgColor);
    int16_t fillW = (int16_t)(progress * (w - 2));
    if (fillW > 0) {
        tft.fillRoundRect(x + 1, y + 1, fillW, h - 2, r - 1, fillColor);
    }
    tft.drawRoundRect(x, y, w, h, r, borderColor);
}

void drawToggle(TFT_eSPI& tft, int16_t x, int16_t y,
                bool state, uint16_t onColor, uint16_t offColor) {
    constexpr int16_t TW = 24, TH = 12;
    tft.fillRoundRect(x, y, TW, TH, TH / 2, state ? onColor : offColor);
    int16_t knobX = state ? (x + TW - TH) : x;
    tft.fillCircle(knobX + TH / 2, y + TH / 2, (TH / 2) - 1, TFT_WHITE);
}

void drawScrollbar(TFT_eSPI& tft,
                   int16_t x, int16_t y, int16_t h,
                   uint8_t total, uint8_t visible, uint8_t offset,
                   uint16_t trackColor, uint16_t thumbColor) {
    if (total <= visible) return;
    tft.drawFastVLine(x, y, h, trackColor);
    int16_t thumbH = max((int16_t)4, (int16_t)((float)visible / total * h));
    int16_t thumbY = y + (int16_t)((float)offset / total * h);
    thumbY = constrain(thumbY, y, y + h - thumbH);
    tft.fillRect(x - 1, thumbY, 3, thumbH, thumbColor);
}

void drawSignalBars(TFT_eSPI& tft, int16_t x, int16_t y,
                    uint8_t strength, uint16_t activeColor, uint16_t inactiveColor) {
    // 4 bars of increasing height (3, 5, 7, 9 px), 3 px wide, 1 px gap
    for (uint8_t i = 0; i < 4; i++) {
        int16_t bh = 3 + i * 2;
        int16_t bx = x + i * 4;
        int16_t by = y + (9 - bh);
        tft.fillRect(bx, by, 3, bh, (i < strength) ? activeColor : inactiveColor);
    }
}

void drawWifiIcon(TFT_eSPI& tft, int16_t cx, int16_t cy,
                  bool connected, int8_t strength, uint16_t color) {
    if (!connected) {
        // Draw a simple disconnected symbol (diagonal cross)
        tft.drawLine(cx - 4, cy - 4, cx + 4, cy + 4, color);
        tft.drawLine(cx + 4, cy - 4, cx - 4, cy + 4, color);
        return;
    }
    // Dot at base
    tft.fillCircle(cx, cy + 5, 1, color);

    // WiFi arcs: drawArc(x, y, r, ir, startAngle, endAngle, color, bg)
    // TFT_eSPI angles: 0=12-o'clock, clockwise. Upper arc = 310° to 50°.
    uint16_t dimC = (uint16_t)(((color >> 11) / 2) << 11 |
                               (((color >> 5) & 0x3F) / 2) << 5 |
                               ((color & 0x1F) / 2));
    if (strength >= 1) tft.drawArc(cx, cy + 6, 4, 2, 310, 50, color, 0x0000);
    if (strength >= 2) tft.drawArc(cx, cy + 6, 7, 5, 310, 50, color, 0x0000);
    if (strength >= 3) tft.drawArc(cx, cy + 6, 10, 8, 310, 50, color, 0x0000);
    (void)dimC;
}

void drawSDIcon(TFT_eSPI& tft, int16_t x, int16_t y, bool present, uint16_t color) {
    // 8×10 SD card silhouette
    tft.drawRect(x, y, 8, 10, color);
    tft.drawLine(x, y + 2, x + 2, y, color);  // notched corner
    if (present) {
        tft.drawFastHLine(x + 2, y + 4, 4, color);
        tft.drawFastHLine(x + 2, y + 6, 4, color);
    }
}

void drawBatteryIcon(TFT_eSPI& tft, int16_t x, int16_t y,
                     uint8_t percent, bool charging, uint16_t color) {
    // Outline: 14×7 + 2 px positive terminal
    tft.drawRect(x, y, 13, 7, color);
    tft.fillRect(x + 13, y + 2, 2, 3, color);
    int16_t fillW = max((int16_t)0, (int16_t)((percent / 100.0f) * 11));
    if (fillW) {
        uint16_t fc = (percent <= 20) ? TFT_RED : color;
        tft.fillRect(x + 1, y + 1, fillW, 5, fc);
    }
    if (charging) {
        tft.drawLine(x + 7, y + 1, x + 5, y + 4, TFT_YELLOW);
        tft.drawLine(x + 5, y + 4, x + 7, y + 4, TFT_YELLOW);
        tft.drawLine(x + 7, y + 4, x + 5, y + 6, TFT_YELLOW);
    }
}

void drawCheckmark(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.drawLine(cx - 4, cy,     cx - 1, cy + 3, color);
    tft.drawLine(cx - 1, cy + 3, cx + 4, cy - 3, color);
}

void drawCrossMark(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.drawLine(cx - 3, cy - 3, cx + 3, cy + 3, color);
    tft.drawLine(cx + 3, cy - 3, cx - 3, cy + 3, color);
}

void drawDivider(TFT_eSPI& tft, int16_t y, uint16_t color) {
    tft.drawFastHLine(4, y, SCREEN_WIDTH - 8, color);
}

} // namespace Widgets
