#include "src/core/NotificationManager.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include "src/board_config.h"
#include <string.h>
#include <math.h>

static constexpr int16_t TOAST_H      = 18;
static constexpr int16_t TOAST_X      = 6;
static constexpr int16_t TOAST_W      = SCREEN_WIDTH - 12;
static constexpr int16_t TOAST_Y_SHOW = SCREEN_HEIGHT - TOAST_H - 4;
static constexpr int16_t TOAST_Y_HIDE = SCREEN_HEIGHT + 2;
static constexpr uint16_t SLIDE_MS    = 150;

NotificationManager& NotificationManager::instance() {
    static NotificationManager inst;
    return inst;
}

void NotificationManager::init() {
    memset(_queue, 0, sizeof(_queue));
    _qHead = _qTail = _qCount = 0;
    _showing = false;
    LOG_INFO("NotificationManager: init OK");
}

void NotificationManager::push(const char* message, NotifIcon icon, uint32_t durationMs) {
    if (_qCount >= NOTIFICATION_MAX_QUEUE) {
        LOG_WARN("NotificationManager: queue full");
        return;
    }
    Notification& n = _queue[_qTail];
    strncpy(n.message, message, sizeof(n.message) - 1);
    n.message[sizeof(n.message) - 1] = '\0';
    n.icon       = icon;
    n.durationMs = durationMs;
    _qTail = (_qTail + 1) % NOTIFICATION_MAX_QUEUE;
    _qCount++;

    if (!_showing) showNext();
}

void NotificationManager::showNext() {
    if (!_qCount) { _showing = false; return; }
    _showing    = true;
    _durMs      = _queue[_qHead].durationMs;
    _timerMs    = millis();
    _yPos       = TOAST_Y_HIDE;
    _slidingIn  = true;
    _slidingOut = false;
}

void NotificationManager::update() {
    if (!_showing) return;
    uint32_t elapsed = millis() - _timerMs;

    if (_slidingIn) {
        float t  = min(1.0f, (float)elapsed / SLIDE_MS);
        _yPos    = (int16_t)(TOAST_Y_HIDE + t * (TOAST_Y_SHOW - TOAST_Y_HIDE));
        if (t >= 1.0f) { _yPos = TOAST_Y_SHOW; _slidingIn = false; }
        return;
    }

    if (!_slidingOut && elapsed >= _durMs) {
        _slidingOut = true;
        _timerMs    = millis();
        return;
    }

    if (_slidingOut) {
        float t  = min(1.0f, (float)elapsed / SLIDE_MS);
        _yPos    = (int16_t)(TOAST_Y_SHOW + t * (TOAST_Y_HIDE - TOAST_Y_SHOW));
        if (t >= 1.0f) {
            _slidingOut = false;
            _showing    = false;
            _qHead  = (_qHead + 1) % NOTIFICATION_MAX_QUEUE;
            _qCount = (_qCount > 0) ? _qCount - 1 : 0;
            showNext();
        }
    }
}

void NotificationManager::render(TFT_eSPI& tft) {
    if (!_showing || !_qCount) return;

    const ThemeColors& c = Theme::instance().colors();
    const Notification& n = _queue[_qHead];

    tft.fillRoundRect(TOAST_X, _yPos, TOAST_W, TOAST_H, 4, c.surface);
    tft.drawRoundRect(TOAST_X, _yPos, TOAST_W, TOAST_H, 4, c.primary);

    tft.setTextColor(c.text);
    int16_t ty = _yPos + (TOAST_H - 8) / 2;
    tft.drawString(n.message, TOAST_X + 6, ty, 1);
}
