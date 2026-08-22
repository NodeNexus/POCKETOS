#include "src/core/SettingsManager.h"
#include "src/debug.h"
#include "src/system_config.h"

SettingsManager& SettingsManager::instance() {
    static SettingsManager inst;
    return inst;
}

bool SettingsManager::init() {
    LOG_INFO("SettingsManager: loading from NVS namespace '%s'", NVS_NAMESPACE);
    loadFromNVS();
    return true;
}

void SettingsManager::loadFromNVS() {
    _prefs.begin(NVS_NAMESPACE, true /* read-only */);

    _brightness   = _prefs.getUChar ("brightness",   SettingsDefaults::BRIGHTNESS);
    _rotation     = _prefs.getUChar ("rotation",     SettingsDefaults::ROTATION);
    _theme        = (ThemeId)_prefs.getUChar("theme", (uint8_t)SettingsDefaults::THEME);
    _wifiEnabled  = _prefs.getBool  ("wifi_en",      SettingsDefaults::WIFI_ENABLED);
    _wifiSSID     = _prefs.getString("wifi_ssid",    "");
    _wifiPassword = _prefs.getString("wifi_pass",    "");
    _sleepTimeout = _prefs.getULong ("sleep_ms",     SettingsDefaults::SLEEP_TIMEOUT);
    _animations   = _prefs.getBool  ("animations",   SettingsDefaults::ANIMATIONS);
    _firstBoot    = _prefs.getBool  ("first_boot",   SettingsDefaults::FIRST_BOOT);
    _deviceName   = _prefs.getString("device_name",  "PocketOS");
    _timezone     = _prefs.getString("timezone",     "UTC0");

    _prefs.end();

    // Bounds-check the theme ID in case of corrupt NVS
    if ((uint8_t)_theme >= (uint8_t)ThemeId::THEME_COUNT) {
        LOG_WARN("SettingsManager: invalid theme in NVS, resetting to DARK");
        _theme = ThemeId::DARK;
    }

    LOG_INFO("SettingsManager: loaded (theme=%d, wifi=%d, brightness=%d, firstBoot=%d)",
             (int)_theme, _wifiEnabled, _brightness, _firstBoot);
}

void SettingsManager::save() {
    if (!_dirty) return;

    LOG_INFO("SettingsManager: saving to NVS");
    _prefs.begin(NVS_NAMESPACE, false /* read-write */);

    _prefs.putUChar ("brightness",   _brightness);
    _prefs.putUChar ("rotation",     _rotation);
    _prefs.putUChar ("theme",        (uint8_t)_theme);
    _prefs.putBool  ("wifi_en",      _wifiEnabled);
    _prefs.putString("wifi_ssid",    _wifiSSID);
    _prefs.putString("wifi_pass",    _wifiPassword);  // plain-text Phase 1
    _prefs.putULong ("sleep_ms",     _sleepTimeout);
    _prefs.putBool  ("animations",   _animations);
    _prefs.putBool  ("first_boot",   _firstBoot);
    _prefs.putString("device_name",  _deviceName);
    _prefs.putString("timezone",     _timezone);

    _prefs.end();
    _dirty = false;
}

void SettingsManager::resetToDefaults() {
    LOG_WARN("SettingsManager: factory reset");
    _brightness  = SettingsDefaults::BRIGHTNESS;
    _rotation    = SettingsDefaults::ROTATION;
    _theme       = SettingsDefaults::THEME;
    _wifiEnabled = SettingsDefaults::WIFI_ENABLED;
    _wifiSSID    = "";
    _wifiPassword= "";
    _sleepTimeout= SettingsDefaults::SLEEP_TIMEOUT;
    _animations  = SettingsDefaults::ANIMATIONS;
    _firstBoot   = true;
    _deviceName  = "PocketOS";
    _timezone    = "UTC0";
    _dirty = true;
    save();
}

void SettingsManager::setBrightness(uint8_t v)        { _brightness  = v;  _dirty = true; }
void SettingsManager::setRotation(uint8_t v)          { _rotation    = v;  _dirty = true; }
void SettingsManager::setAnimations(bool v)           { _animations  = v;  _dirty = true; }
void SettingsManager::setTheme(ThemeId id)            { _theme       = id; _dirty = true; }
void SettingsManager::setWifiEnabled(bool v)          { _wifiEnabled = v;  _dirty = true; }
void SettingsManager::setSleepTimeout(uint32_t ms)    { _sleepTimeout= ms; _dirty = true; }
void SettingsManager::clearFirstBoot()                { _firstBoot   = false; _dirty = true; }
void SettingsManager::setDeviceName(const String& n)  { _deviceName  = n;  _dirty = true; }
void SettingsManager::setTimezone(const String& tz)   { _timezone    = tz; _dirty = true; }

void SettingsManager::setWifiCredentials(const String& ssid, const String& pwd) {
    _wifiSSID     = ssid;
    _wifiPassword = pwd;   // LOG_SENSITIVE("wifi_password") — never print this
    _dirty = true;
}
