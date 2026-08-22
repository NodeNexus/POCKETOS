#include "src/apps/PlaceholderApp.h"
#include "src/ui/StatusBar.h"
#include <string.h>

PlaceholderApp::PlaceholderApp(uint8_t id, const char* name, const char* phase,
                               void (*iconDraw)(TFT_eSPI&, int16_t, int16_t, uint16_t)) {
    _info.id       = id;
    _info.name     = name;
    _info.drawIcon = iconDraw;
    strncpy(_phase, phase, sizeof(_phase) - 1);
    _phase[sizeof(_phase) - 1] = '\0';
}

void PlaceholderApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    if (_rendered) return;
    _rendered = true;

    const ThemeColors& c = Theme::instance().colors();

    // Content area
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    // Header bar
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
    tft.setTextColor(c.primary);
    int16_t tw = tft.textWidth(_info.name, 2);
    tft.drawString(_info.name, (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 2, 2);
    tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 18, SCREEN_WIDTH, c.border);

    // Large icon centred on screen
    if (_info.drawIcon) {
        _info.drawIcon(tft, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 4, c.textDisabled);
    }

    // "COMING SOON" text
    tft.setTextColor(c.textMuted);
    const char* l1 = "COMING SOON";
    const char* l2 = _phase;
    tft.drawString(l1, (SCREEN_WIDTH - tft.textWidth(l1, 1)) / 2,
                   SCREEN_HEIGHT / 2 + 16, 1);
    tft.setTextColor(c.textDisabled);
    tft.drawString(l2, (SCREEN_WIDTH - tft.textWidth(l2, 1)) / 2,
                   SCREEN_HEIGHT / 2 + 27, 1);

    // Hint
    const char* hint = "Hold SEL \xb7 Back";   // centre-dot
    tft.drawString(hint, (SCREEN_WIDTH - tft.textWidth(hint, 1)) / 2,
                   SCREEN_HEIGHT - 11, 1);
}

void PlaceholderApp::handleInput(AppEvent event) {
    // AppManager handles BTN_SELECT_LONG → exit; nothing extra needed here
    (void)event;
}
