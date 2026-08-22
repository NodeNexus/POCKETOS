#pragma once
// =============================================================================
// InputManager.h — Physical button polling and event generation
//
// Polls three buttons (UP, SELECT, DOWN) every loop() using millis()-based
// debouncing. Generates typed AppEvent values into a small circular queue.
// AppManager reads the queue and forwards events to the active App.
//
// Detects: short press, long press, very-long press, auto-repeat while held.
// =============================================================================

#include <Arduino.h>
#include "src/apps/App.h"           // AppEvent
#include "src/board_config.h"
#include "src/system_config.h"

// Circular event queue capacity
static constexpr uint8_t INPUT_QUEUE_SIZE = 16;

// Internal per-button state
struct ButtonState {
    bool     pressed;           // Debounced current state
    bool     lastRaw;           // Raw GPIO read last frame
    uint32_t pressStartMs;      // When current press began (debounced edge)
    uint32_t lastDebounceMs;    // Debounce timer reference
    uint32_t lastRepeatMs;      // Auto-repeat reference
    bool     shortFired;        // Short-press event already sent this press
    bool     longFired;         // Long-press event already sent this press
    bool     veryLongFired;     // Very-long event already sent this press
    bool     repeatActive;      // Auto-repeat phase started
};

class InputManager {
public:
    static InputManager& instance();

    // Initialise GPIO pins with the correct pull direction
    void init();

    // Poll all buttons; call once per loop() before AppManager::update()
    void update();

    // Query the event queue
    bool     hasEvent()  const;
    AppEvent popEvent();            // Dequeue; returns NONE if empty
    AppEvent peekEvent() const;     // Peek without dequeue
    void     clearEvents();         // Flush queue (called on app transitions)

    // Debounced state queries (useful for "is currently held" checks)
    bool     isUpPressed()     const { return _up.pressed; }
    bool     isDownPressed()   const { return _down.pressed; }
    bool     isSelectPressed() const { return _select.pressed; }

    // Milliseconds button has been continuously held (0 if not pressed)
    uint32_t upHeldMs()     const;
    uint32_t downHeldMs()   const;
    uint32_t selectHeldMs() const;

private:
    InputManager() = default;

    void pollButton(ButtonState& state, uint8_t pin, uint8_t index);
    void pushEvent(AppEvent evt);

    ButtonState _up;
    ButtonState _down;
    ButtonState _select;

    AppEvent _queue[INPUT_QUEUE_SIZE];
    uint8_t  _qHead  = 0;
    uint8_t  _qTail  = 0;
    uint8_t  _qCount = 0;
};
