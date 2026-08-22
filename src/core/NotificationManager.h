#pragma once
// =============================================================================
// NotificationManager.h — Non-blocking toast notification overlay
//
// Toasts slide in from the bottom, display for a configurable duration, then
// slide out — without disturbing the active app's state.
// Queue holds up to NOTIFICATION_MAX_QUEUE pending toasts.
// =============================================================================

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "src/system_config.h"

enum class NotifIcon : uint8_t {
    NONE = 0,
    WIFI_ON, WIFI_OFF,
    SD_OK, SD_ERROR,
    BATTERY_LOW,
    TIMER,
    INFO,
    WARNING,
    ERROR_ICON,
};

struct Notification {
    char      message[48];
    NotifIcon icon;
    uint32_t  durationMs;
};

class NotificationManager {
public:
    static NotificationManager& instance();

    void init();

    // Call every loop() to advance slide animations
    void update();

    // Call every render frame to draw any active toast
    void render(TFT_eSPI& tft);

    // Queue a notification (oldest dropped if queue full)
    void push(const char* message,
              NotifIcon icon      = NotifIcon::NONE,
              uint32_t durationMs = NOTIFICATION_TOAST_MS);

    bool hasActive() const { return _showing; }

private:
    NotificationManager() = default;

    void showNext();

    Notification _queue[NOTIFICATION_MAX_QUEUE];
    uint8_t  _qHead      = 0;
    uint8_t  _qTail      = 0;
    uint8_t  _qCount     = 0;

    bool     _showing    = false;
    uint32_t _timerMs    = 0;
    uint32_t _durMs      = 0;
    int16_t  _yPos       = 0;      // Current toast Y (animated)
    bool     _slidingIn  = false;
    bool     _slidingOut = false;
};
