#include "src/core/SysNetworkManager.h"
#include "src/debug.h"
#include "src/system_config.h"

SysNetworkManager& SysNetworkManager::instance() {
    static SysNetworkManager inst;
    return inst;
}

void SysNetworkManager::init(bool enabled) {
    _enabled = enabled;
    if (!enabled) {
        WiFi.mode(WIFI_OFF);
        _state = WifiState::WIFI_OFF;
        LOG_INFO("SysNetworkManager: WiFi disabled");
        return;
    }
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(false);  // Managed manually
    _state = WifiState::DISCONNECTED;
    LOG_INFO("SysNetworkManager: WiFi enabled  MAC=%s", WiFi.macAddress().c_str());
}

bool SysNetworkManager::connect(const char* ssid, const char* password) {
    if (!_enabled) { LOG_WARN("SysNetworkManager: WiFi disabled"); return false; }

    LOG_INFO("SysNetworkManager: connecting to '%s'", ssid);
    LOG_SENSITIVE("wifi_password");   // Never log the password

    _state          = WifiState::CONNECTING;
    _connectStartMs = millis();
    WiFi.begin(ssid, password);

    // Phase 1: blocking connect with timeout
    // Phase 3: make this non-blocking with a state machine
    while (WiFi.status() != WL_CONNECTED &&
           millis() - _connectStartMs < WIFI_CONNECT_TIMEOUT_MS) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        _state = WifiState::CONNECTED;
        LOG_INFO("SysNetworkManager: connected  IP=%s", WiFi.localIP().toString().c_str());
        return true;
    }

    _state = WifiState::DISCONNECTED;
    WiFi.disconnect();
    LOG_WARN("SysNetworkManager: connection failed");
    return false;
}

void SysNetworkManager::disconnect() {
    WiFi.disconnect();
    _state = _enabled ? WifiState::DISCONNECTED : WifiState::WIFI_OFF;
    LOG_INFO("SysNetworkManager: disconnected");
}

void SysNetworkManager::update() {
    if (!_enabled || _state == WifiState::WIFI_OFF) return;
    if (_state == WifiState::CONNECTED && WiFi.status() != WL_CONNECTED) {
        _state = WifiState::DISCONNECTED;
        LOG_WARN("SysNetworkManager: connection lost");
    }
}

String  SysNetworkManager::getSSID()       const { return isConnected() ? WiFi.SSID()                       : ""; }
String  SysNetworkManager::getLocalIPStr() const { return isConnected() ? WiFi.localIP().toString()         : ""; }
String  SysNetworkManager::getMacAddress() const { return WiFi.macAddress(); }
int32_t SysNetworkManager::getRSSI()       const { return isConnected() ? WiFi.RSSI()    : 0; }
uint8_t SysNetworkManager::getChannel()    const { return isConnected() ? WiFi.channel() : 0; }

int SysNetworkManager::scanNetworks() {
    LOG_INFO("SysNetworkManager: scanning...");
    int n = WiFi.scanNetworks();
    LOG_INFO("SysNetworkManager: found %d networks", n);
    return n;
}
