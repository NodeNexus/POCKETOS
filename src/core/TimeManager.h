#pragma once
// =============================================================================
// TimeManager.h — System time via NTP or millis()-based uptime fallback
//
// Phase 1: init() configures timezone; syncNTP() must be called after Wi-Fi
// connects. If no sync, getTimeString() returns uptime-derived HH:MM.
// =============================================================================

#include <Arduino.h>
#include <time.h>

class TimeManager {
public:
    static TimeManager& instance();

    // Configure timezone; call in setup() before syncNTP()
    void init(const char* timezone = "UTC0");

    // Attempt NTP sync — call after Wi-Fi connects (blocks briefly with delay)
    bool syncNTP();

    bool isSynced() const { return _synced; }

    // Formatted time strings (fill provided buffer)
    void getTimeString    (char* buf, size_t len);  // "14:35"
    void getDateString    (char* buf, size_t len);  // "01 Aug"
    void getFullDateString(char* buf, size_t len);  // "2026-08-01"
    void getUptimeString  (char* buf, size_t len);  // "2d 4h 32m"

    time_t   getEpoch()          const;
    uint32_t uptimeSeconds()     const { return millis() / 1000; }

private:
    TimeManager() = default;
    bool   _synced   = false;
    String _timezone = "UTC0";
};
