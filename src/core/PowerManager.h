#pragma once
// =============================================================================
// PowerManager.h — Display power states and inactivity management
//
// State machine:  ACTIVE → DIMMED → SLEEP
// Any button press calls reportActivity() to reset the inactivity timer.
// Battery querying is a stub in Phase 1 (hardware not connected).
// =============================================================================

#include <Arduino.h>
#include "src/system_config.h"

enum class PowerState : uint8_t {
    ACTIVE = 0,
    DIMMED,
    SLEEP
};

class PowerManager {
public:
    static PowerManager& instance();

    void init(uint32_t dimTimeoutMs   = POWER_DIM_TIMEOUT_MS,
              uint32_t sleepTimeoutMs = POWER_SLEEP_TIMEOUT_MS);

    // Call every loop() to advance the state machine
    void update();

    // Reset inactivity timer; if sleeping, wake the display
    void reportActivity();

    PowerState getState()   const { return _state; }
    bool       isActive()   const { return _state == PowerState::ACTIVE; }
    bool       isSleeping() const { return _state == PowerState::SLEEP; }

    void setDimTimeout  (uint32_t ms) { _dimTimeoutMs   = ms; }
    void setSleepTimeout(uint32_t ms) { _sleepTimeoutMs = ms; }

    // -----------------------------------------------------------------------
    // Battery (Phase 1 stubs — hardware not connected)
    // -----------------------------------------------------------------------
    bool    hasBattery()        const { return false; }
    uint8_t getBatteryPercent() const { return 100; }
    bool    isCharging()        const { return false; }

private:
    PowerManager() = default;

    void enterDimmed();
    void enterSleep();
    void exitSleep();

    PowerState _state          = PowerState::ACTIVE;
    uint32_t   _lastActivityMs = 0;
    uint32_t   _dimTimeoutMs   = POWER_DIM_TIMEOUT_MS;
    uint32_t   _sleepTimeoutMs = POWER_SLEEP_TIMEOUT_MS;
};
