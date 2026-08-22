#include "src/ui/Dialog.h"
#include <string.h>

void Dialog::show(const char* title, const char* message,
                  const char* confirmLabel, const char* cancelLabel,
                  bool focusConfirm) {
    strncpy(_title,        title,        sizeof(_title)        - 1);
    strncpy(_message,      message,      sizeof(_message)      - 1);
    strncpy(_confirmLabel, confirmLabel, sizeof(_confirmLabel) - 1);
    strncpy(_cancelLabel,  cancelLabel,  sizeof(_cancelLabel)  - 1);
    _title       [sizeof(_title)        - 1] = '\0';
    _message     [sizeof(_message)      - 1] = '\0';
    _confirmLabel[sizeof(_confirmLabel) - 1] = '\0';
    _cancelLabel [sizeof(_cancelLabel)  - 1] = '\0';

    _active   = true;
    _result   = DialogResult::PENDING;
    _focusYes = focusConfirm;
}

void Dialog::handleInput(AppEvent event) {
    if (!_active) return;
    switch (event) {
        case AppEvent::BTN_UP_SHORT:
        case AppEvent::BTN_DOWN_SHORT:
            _focusYes = !_focusYes;
            break;
        case AppEvent::BTN_SELECT_SHORT:
            _result = _focusYes ? DialogResult::CONFIRMED : DialogResult::CANCELLED;
            _active = false;
            break;
        case AppEvent::BTN_SELECT_LONG:
            _result = DialogResult::CANCELLED;
            _active = false;
            break;
        default: break;
    }
}

void Dialog::render(TFT_eSPI& tft) {
    if (!_active) return;

    const ThemeColors& c = Theme::instance().colors();

    // Dialog box (centred, with padding)
    constexpr int16_t DX = 10, DY = 28, DW = SCREEN_WIDTH - 20, DH = 72;
    constexpr int16_t R  = 6;

    tft.fillRoundRect(DX, DY, DW, DH, R, c.background);
    tft.drawRoundRect(DX, DY, DW, DH, R, c.primary);

    // Title
    tft.setTextColor(c.primary);
    int16_t tw = tft.textWidth(_title, 2);
    tft.drawString(_title, DX + (DW - tw) / 2, DY + 6, 2);

    // Divider
    tft.drawFastHLine(DX + 4, DY + 22, DW - 8, c.border);

    // Message
    tft.setTextColor(c.text);
    int16_t mw = tft.textWidth(_message, 1);
    if (mw < DW - 8) {
        tft.drawString(_message, DX + (DW - mw) / 2, DY + 28, 1);
    } else {
        // Truncate long messages — Phase 9: add word-wrap
        char buf[28];
        strncpy(buf, _message, 27);
        buf[27] = '\0';
        tft.drawString(buf, DX + 4, DY + 28, 1);
    }

    // Buttons
    constexpr int16_t BTN_W = 44, BTN_H = 16, BTN_Y = DY + 48;
    int16_t cancelX  = DX + 6;
    int16_t confirmX = DX + DW - 6 - BTN_W;

    // Cancel button
    uint16_t noBg  = _focusYes ? c.surface : c.selection;
    uint16_t noBdr = _focusYes ? c.border  : c.selectionBorder;
    tft.fillRoundRect(cancelX,  BTN_Y, BTN_W, BTN_H, 3, noBg);
    tft.drawRoundRect(cancelX,  BTN_Y, BTN_W, BTN_H, 3, noBdr);
    tft.setTextColor(_focusYes ? c.textMuted : c.text);
    {
        int16_t lw = tft.textWidth(_cancelLabel, 1);
        tft.drawString(_cancelLabel, cancelX + (BTN_W - lw) / 2, BTN_Y + 4, 1);
    }

    // Confirm button
    uint16_t yesBg  = _focusYes ? c.selection : c.surface;
    uint16_t yesBdr = _focusYes ? c.selectionBorder : c.border;
    tft.fillRoundRect(confirmX, BTN_Y, BTN_W, BTN_H, 3, yesBg);
    tft.drawRoundRect(confirmX, BTN_Y, BTN_W, BTN_H, 3, yesBdr);
    tft.setTextColor(_focusYes ? c.text : c.textMuted);
    {
        int16_t lw = tft.textWidth(_confirmLabel, 1);
        tft.drawString(_confirmLabel, confirmX + (BTN_W - lw) / 2, BTN_Y + 4, 1);
    }
}
