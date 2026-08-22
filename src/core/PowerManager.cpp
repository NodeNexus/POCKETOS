#include "src/core/PowerManager.h"
#include "src/core/DisplayManager.h"
#include "src/core/SettingsManager.h"
#include "src/debug.h"
#include "src/board_config.h"

PowerManager& PowerManager::instance() {
    static PowerManager inst;
    return inst;
}

void PowerManager::init(uint32_t dimTimeoutMs, uint32_t sleepTimeoutMs) {
    _dimTimeoutMs   = dimTimeoutMs;
    _sleepTimeoutMs = sleepTimeoutMs;
    _lastActivityMs = millis();
    _state          = PowerState::ACTIVE;
    LOG_INFO("PowerManager: dim=%lums  sleep=%lums",
             (unsigned long)dimTimeoutMs, (unsigned long)sleepTimeoutMs);
}

void PowerManager::reportActivity() {
    _lastActivityMs = millis();
    if (_state != PowerState::ACTIVE) {
        exitSleep();
    }
}

void PowerManager::update() {
    uint32_t idleMs = millis() - _lastActivityMs;

    switch (_state) {
        case PowerState::ACTIVE:
            if (idleMs >= _dimTimeoutMs) enterDimmed();
            break;
        case PowerState::DIMMED:
            if (idleMs >= _sleepTimeoutMs) enterSleep();
            break;
        case PowerState::SLEEP:
            break;  // Only reportActivity() exits sleep
    }
}

void PowerManager::enterDimmed() {
    if (_state == PowerState::DIMMED) return;
    _state = PowerState::DIMMED;
    LOG_INFO("PowerManager: → DIMMED");
    DisplayManager::instance().setBrightness(POWER_DIM_BRIGHTNESS);
}

void PowerManager::enterSleep() {
    if (_state == PowerState::SLEEP) return;
    _state = PowerState::SLEEP;
    LOG_INFO("PowerManager: → SLEEP");
    DisplayManager::instance().setBacklightOn(false);
    // NOTE: ESP32 light sleep with button wakeup is deferred until
    // physical hardware testing confirms GPIO wakeup behaviour.
    // esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_BUTTON_SELECT, LOW);
    // esp_light_sleep_start();
}

void PowerManager::exitSleep() {
    LOG_INFO("PowerManager: waking");
    _state = PowerState::ACTIVE;
    DisplayManager::instance().setBrightness(
        SettingsManager::instance().getBrightness());
}
