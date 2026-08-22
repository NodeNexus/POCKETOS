#ifndef SYS_NETWORK_MANAGER_H
#define SYS_NETWORK_MANAGER_H
// =============================================================================
// SysNetworkManager.h — Wi-Fi connection management
//
// Phase 1: Init hardware, expose status. Blocking connect() for setup.
// Phase 3: Non-blocking connect, scan UI, saved networks.
// =============================================================================

#include <Arduino.h>
#include <WiFi.h>

enum class WifiState : uint8_t {
    WIFI_OFF     = 0,    // Renamed from DISABLED — Arduino #defines DISABLED as a macro
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    AP_MODE,
    WIFI_ERROR
};

class SysNetworkManager {
public:
    static SysNetworkManager& instance();

    void init(bool enabled);
    void update();   // Check for drops; call each loop()

    bool connect   (const char* ssid, const char* password);
    void disconnect();

    WifiState  getState()     const { return _state; }
    bool       isConnected()  const { return _state == WifiState::CONNECTED; }
    bool       isEnabled()    const { return _enabled; }

    // Network information (valid only when connected)
    String     getSSID()      const;
    String     getLocalIPStr()const;
    String     getMacAddress()const;
    int32_t    getRSSI()      const;
    uint8_t    getChannel()   const;

    // Scan (blocking; Phase 3 will add async variant)
    int        scanNetworks();

private:
    SysNetworkManager() = default;

    bool      _enabled        = false;
    WifiState _state          = WifiState::WIFI_OFF;
    uint32_t  _connectStartMs = 0;
};

#endif // SYS_NETWORK_MANAGER_H
