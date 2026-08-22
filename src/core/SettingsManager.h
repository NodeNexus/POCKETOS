#pragma once
// =============================================================================
// SettingsManager.h — Persistent user settings via ESP32 NVS (Preferences)
//
// All settings have safe defaults. Loaded once at boot; saved explicitly
// (never every frame). Use save() to commit pending changes.
// NVS key names must be ≤15 characters.
// =============================================================================

#include <Arduino.h>
#include <Preferences.h>
#include "src/ui/Theme.h"

// ---------------------------------------------------------------------------
// Compile-time defaults (also used after factory reset)
// ---------------------------------------------------------------------------
namespace SettingsDefaults {
    constexpr uint8_t  BRIGHTNESS      = 180;
    constexpr uint8_t  ROTATION        = 0;
    constexpr ThemeId  THEME           = ThemeId::DARK;
    constexpr bool     WIFI_ENABLED    = false;
    constexpr uint32_t SLEEP_TIMEOUT   = 60000;   // ms
    constexpr bool     ANIMATIONS      = true;
    constexpr bool     FIRST_BOOT      = true;
}

class SettingsManager {
public:
    static SettingsManager& instance();

    bool init();      // Load from NVS; returns true always (uses defaults on failure)
    void save();      // Write dirty settings to NVS; no-op if nothing changed
    void resetToDefaults();

    // -----------------------------------------------------------------------
    // Display
    // -----------------------------------------------------------------------
    uint8_t getBrightness()    const { return _brightness; }
    void    setBrightness(uint8_t v);

    uint8_t getRotation()      const { return _rotation; }
    void    setRotation(uint8_t v);

    bool    getAnimations()    const { return _animations; }
    void    setAnimations(bool v);

    // -----------------------------------------------------------------------
    // Theme
    // -----------------------------------------------------------------------
    ThemeId getTheme()         const { return _theme; }
    void    setTheme(ThemeId id);

    // -----------------------------------------------------------------------
    // Wi-Fi
    // -----------------------------------------------------------------------
    bool    getWifiEnabled()   const { return _wifiEnabled; }
    void    setWifiEnabled(bool v);

    // Credentials: never printed. Use LOG_SENSITIVE() around any debug output.
    String  getWifiSSID()      const { return _wifiSSID; }
    String  getWifiPassword()  const { return _wifiPassword; }
    void    setWifiCredentials(const String& ssid, const String& password);

    // -----------------------------------------------------------------------
    // Power
    // -----------------------------------------------------------------------
    uint32_t getSleepTimeout() const { return _sleepTimeout; }
    void     setSleepTimeout(uint32_t ms);

    // -----------------------------------------------------------------------
    // System
    // -----------------------------------------------------------------------
    bool    isFirstBoot()      const { return _firstBoot; }
    void    clearFirstBoot();

    String  getDeviceName()    const { return _deviceName; }
    void    setDeviceName(const String& name);

    String  getTimezone()      const { return _timezone; }
    void    setTimezone(const String& tz);

    bool    isDirty()          const { return _dirty; }

private:
    SettingsManager() = default;
    void loadFromNVS();

    Preferences _prefs;

    uint8_t  _brightness    = SettingsDefaults::BRIGHTNESS;
    uint8_t  _rotation      = SettingsDefaults::ROTATION;
    ThemeId  _theme         = SettingsDefaults::THEME;
    bool     _wifiEnabled   = SettingsDefaults::WIFI_ENABLED;
    String   _wifiSSID;
    String   _wifiPassword;
    uint32_t _sleepTimeout  = SettingsDefaults::SLEEP_TIMEOUT;
    bool     _animations    = SettingsDefaults::ANIMATIONS;
    bool     _firstBoot     = SettingsDefaults::FIRST_BOOT;
    String   _deviceName    = "PocketOS";
    String   _timezone      = "UTC0";
    bool     _dirty         = false;
};
