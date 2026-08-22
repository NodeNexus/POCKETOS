#include "src/core/TimeManager.h"
#include "src/debug.h"
#include "src/system_config.h"
#include <Arduino.h>

TimeManager& TimeManager::instance() {
    static TimeManager inst;
    return inst;
}

void TimeManager::init(const char* timezone) {
    _timezone = timezone;
    setenv("TZ", timezone, 1);
    tzset();
    LOG_INFO("TimeManager: TZ='%s'", timezone);
}

bool TimeManager::syncNTP() {
    LOG_INFO("TimeManager: NTP sync → %s", NTP_SERVER_1);
    configTime(0, 0, NTP_SERVER_1, NTP_SERVER_2);

    uint32_t start = millis();
    while (millis() - start < NTP_SYNC_TIMEOUT_MS) {
        struct tm t;
        if (getLocalTime(&t, 100)) {   // 100 ms wait inside getLocalTime
            _synced = true;
            char buf[32];
            strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
            LOG_INFO("TimeManager: synced → %s", buf);
            return true;
        }
    }
    LOG_WARN("TimeManager: NTP sync timed out");
    return false;
}

void TimeManager::getTimeString(char* buf, size_t len) {
    if (_synced) {
        struct tm t;
        if (getLocalTime(&t, 0)) { strftime(buf, len, "%H:%M", &t); return; }
    }
    // Fallback: uptime as HH:MM
    uint32_t s = uptimeSeconds();
    snprintf(buf, len, "%02lu:%02lu", (unsigned long)((s / 3600) % 24),
                                      (unsigned long)((s / 60)   % 60));
}

void TimeManager::getDateString(char* buf, size_t len) {
    if (_synced) {
        struct tm t;
        if (getLocalTime(&t, 0)) { strftime(buf, len, "%d %b", &t); return; }
    }
    snprintf(buf, len, "--");
}

void TimeManager::getFullDateString(char* buf, size_t len) {
    if (_synced) {
        struct tm t;
        if (getLocalTime(&t, 0)) { strftime(buf, len, "%Y-%m-%d", &t); return; }
    }
    snprintf(buf, len, "----/--/--");
}

void TimeManager::getUptimeString(char* buf, size_t len) {
    uint32_t total = uptimeSeconds();
    uint32_t d = total / 86400, h = (total % 86400) / 3600;
    uint32_t m = (total % 3600) / 60, s = total % 60;
    if (d) snprintf(buf, len, "%lud %luh %lum", (unsigned long)d, (unsigned long)h, (unsigned long)m);
    else if (h) snprintf(buf, len, "%luh %lum %lus", (unsigned long)h, (unsigned long)m, (unsigned long)s);
    else        snprintf(buf, len, "%lum %lus", (unsigned long)m, (unsigned long)s);
}

time_t TimeManager::getEpoch() const {
    return _synced ? time(nullptr) : 0;
}
