#include "src/core/InputManager.h"
#include "src/debug.h"
#include <string.h>

InputManager& InputManager::instance() {
    static InputManager inst;
    return inst;
}

void InputManager::init() {
    uint8_t mode = BUTTON_ACTIVE_LOW ? INPUT_PULLUP : INPUT_PULLDOWN;
    pinMode(PIN_BUTTON_UP,     mode);
    pinMode(PIN_BUTTON_DOWN,   mode);
    pinMode(PIN_BUTTON_SELECT, mode);

    memset(&_up,     0, sizeof(ButtonState));
    memset(&_down,   0, sizeof(ButtonState));
    memset(&_select, 0, sizeof(ButtonState));

    LOG_INFO("InputManager: UP=%d  SEL=%d  DOWN=%d  active-%s",
             PIN_BUTTON_UP, PIN_BUTTON_SELECT, PIN_BUTTON_DOWN,
             BUTTON_ACTIVE_LOW ? "LOW" : "HIGH");
}

void InputManager::update() {
    pollButton(_up,     PIN_BUTTON_UP,     0);
    pollButton(_down,   PIN_BUTTON_DOWN,   1);
    pollButton(_select, PIN_BUTTON_SELECT, 2);
}

// index: 0=UP  1=DOWN  2=SELECT — used to select the right AppEvent variant
void InputManager::pollButton(ButtonState& s, uint8_t pin, uint8_t index) {
    bool raw = (digitalRead(pin) == (BUTTON_ACTIVE_LOW ? LOW : HIGH));
    uint32_t now = millis();

    // Debounce: ignore transitions shorter than BTN_DEBOUNCE_MS
    if (raw != s.lastRaw) {
        s.lastDebounceMs = now;
        s.lastRaw = raw;
    }
    if ((now - s.lastDebounceMs) < BTN_DEBOUNCE_MS) return;

    bool wasPressed = s.pressed;
    s.pressed = raw;

    // ---- Rising edge (press start) ----
    if (s.pressed && !wasPressed) {
        s.pressStartMs  = now;
        s.lastRepeatMs  = now;
        s.shortFired    = false;
        s.longFired     = false;
        s.veryLongFired = false;
        s.repeatActive  = false;
        LOG_DEBUG("BTN%d pressed", index);
    }

    // ---- While held ----
    if (s.pressed) {
        uint32_t held = now - s.pressStartMs;

        if (!s.veryLongFired && held >= BTN_VERY_LONG_PRESS_MS) {
            // Very-long press — suppress short and long
            s.veryLongFired = s.longFired = s.shortFired = true;
            AppEvent evt = AppEvent::NONE;
            if (index == 2) evt = AppEvent::BTN_SELECT_VERY_LONG;
            if (evt != AppEvent::NONE) pushEvent(evt);

        } else if (!s.longFired && held >= BTN_LONG_PRESS_MS) {
            // Long press
            s.longFired = s.shortFired = true;
            AppEvent evt = AppEvent::NONE;
            switch (index) {
                case 0: evt = AppEvent::BTN_UP_LONG;     break;
                case 1: evt = AppEvent::BTN_DOWN_LONG;   break;
                case 2: evt = AppEvent::BTN_SELECT_LONG; break;
            }
            if (evt != AppEvent::NONE) pushEvent(evt);

        } else if (s.longFired && index != 2) {
            // Auto-repeat for UP/DOWN once long-press threshold passed
            if (!s.repeatActive && held >= BTN_REPEAT_INITIAL_MS) {
                s.repeatActive = true;
                s.lastRepeatMs = now;
            }
            if (s.repeatActive && (now - s.lastRepeatMs) >= BTN_REPEAT_INTERVAL_MS) {
                s.lastRepeatMs = now;
                AppEvent evt = (index == 0) ? AppEvent::BTN_UP_HELD : AppEvent::BTN_DOWN_HELD;
                pushEvent(evt);
            }
        }
    }

    // ---- Falling edge (release) ----
    if (!s.pressed && wasPressed) {
        uint32_t held = now - s.pressStartMs;
        if (!s.shortFired && held < BTN_LONG_PRESS_MS) {
            AppEvent evt = AppEvent::NONE;
            switch (index) {
                case 0: evt = AppEvent::BTN_UP_SHORT;     break;
                case 1: evt = AppEvent::BTN_DOWN_SHORT;   break;
                case 2: evt = AppEvent::BTN_SELECT_SHORT; break;
            }
            if (evt != AppEvent::NONE) {
                pushEvent(evt);
                LOG_DEBUG("BTN%d short-press", index);
            }
        }
    }
}

void InputManager::pushEvent(AppEvent evt) {
    if (_qCount >= INPUT_QUEUE_SIZE) {
        LOG_WARN("InputManager: queue full, dropping event");
        return;
    }
    _queue[_qTail] = evt;
    _qTail = (_qTail + 1) % INPUT_QUEUE_SIZE;
    _qCount++;
}

bool     InputManager::hasEvent()  const { return _qCount > 0; }
AppEvent InputManager::peekEvent() const { return _qCount ? _queue[_qHead] : AppEvent::NONE; }

AppEvent InputManager::popEvent() {
    if (!_qCount) return AppEvent::NONE;
    AppEvent evt = _queue[_qHead];
    _qHead = (_qHead + 1) % INPUT_QUEUE_SIZE;
    _qCount--;
    return evt;
}

void InputManager::clearEvents() {
    _qHead = _qTail = _qCount = 0;
}

uint32_t InputManager::upHeldMs()     const { return _up.pressed     ? millis() - _up.pressStartMs     : 0; }
uint32_t InputManager::downHeldMs()   const { return _down.pressed   ? millis() - _down.pressStartMs   : 0; }
uint32_t InputManager::selectHeldMs() const { return _select.pressed ? millis() - _select.pressStartMs : 0; }
