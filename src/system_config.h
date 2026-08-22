#pragma once
// =============================================================================
// system_config.h — PocketOS System-Wide Constants
// Timing, feature flags, storage paths, and firmware identity.
// Avoids magic numbers scattered throughout the codebase.
// =============================================================================

// ---------------------------------------------------------------------------
// Firmware Identity
// ---------------------------------------------------------------------------
#ifndef POCKETOS_VERSION_MAJOR
  #define POCKETOS_VERSION_MAJOR  0
#endif
#ifndef POCKETOS_VERSION_MINOR
  #define POCKETOS_VERSION_MINOR  1
#endif
#ifndef POCKETOS_VERSION_PATCH
  #define POCKETOS_VERSION_PATCH  0
#endif

#define POCKETOS_DEVICE_NAME    "PocketOS"
#define POCKETOS_AP_SSID        "PocketOS-Setup"   // Wi-Fi AP name for setup portal

// ---------------------------------------------------------------------------
// Input Timing  (milliseconds)
// ---------------------------------------------------------------------------
#define BTN_DEBOUNCE_MS           50
#define BTN_LONG_PRESS_MS         800
#define BTN_VERY_LONG_PRESS_MS    2000
#define BTN_REPEAT_INITIAL_MS     600   // Delay before auto-repeat starts when held
#define BTN_REPEAT_INTERVAL_MS    150   // Repeat interval after initial delay

// ---------------------------------------------------------------------------
// Power Management
// ---------------------------------------------------------------------------
#define POWER_DIM_TIMEOUT_MS      20000  // Dim backlight after 20 s of inactivity
#define POWER_SLEEP_TIMEOUT_MS    60000  // Turn off backlight after 60 s
#define POWER_DIM_BRIGHTNESS      40     // Dimmed brightness level (0-255)

// ---------------------------------------------------------------------------
// Display / Rendering
// ---------------------------------------------------------------------------
#define DISPLAY_TARGET_FPS        30
#define DISPLAY_FRAME_MS          (1000 / DISPLAY_TARGET_FPS)   // ~33 ms per frame

// ---------------------------------------------------------------------------
// SD Card Directory Structure
// ---------------------------------------------------------------------------
#define SD_POCKETOS_DIR   "/PocketOS"
#define SD_CONFIG_DIR     "/PocketOS/config"
#define SD_SAVES_DIR      "/PocketOS/saves"
#define SD_PHOTOS_DIR     "/PocketOS/photos"
#define SD_MUSIC_DIR      "/PocketOS/music"
#define SD_LOGS_DIR       "/PocketOS/logs"
#define SD_CACHE_DIR      "/PocketOS/cache"

// ---------------------------------------------------------------------------
// NVS (Non-Volatile Storage) Namespace
// ---------------------------------------------------------------------------
#define NVS_NAMESPACE     "pocketos"

// ---------------------------------------------------------------------------
// Network
// ---------------------------------------------------------------------------
#define WIFI_CONNECT_TIMEOUT_MS   15000
#define WIFI_SCAN_TIMEOUT_MS       8000
#define NTP_SERVER_1    "pool.ntp.org"
#define NTP_SERVER_2    "time.google.com"
#define NTP_SYNC_TIMEOUT_MS       10000

// ---------------------------------------------------------------------------
// Notifications
// ---------------------------------------------------------------------------
#define NOTIFICATION_TOAST_MS     3000   // Toast display duration
#define NOTIFICATION_MAX_QUEUE    4      // Max queued toasts

// ---------------------------------------------------------------------------
// Boot Sequence
// ---------------------------------------------------------------------------
#define BOOT_STEP_DELAY_MS        300    // Pause between boot steps
#define BOOT_FINISH_PAUSE_MS      400    // Pause after all steps before launcher

// ---------------------------------------------------------------------------
// App Stack
// ---------------------------------------------------------------------------
#define APP_STACK_MAX_DEPTH       6      // Max nested app depth

// ---------------------------------------------------------------------------
// Feature Flags  (0 = disabled, 1 = enabled)
// ---------------------------------------------------------------------------
#define FEATURE_WIFI          1
#define FEATURE_SD            1
#define FEATURE_BATTERY       0    // Not yet connected
#define FEATURE_AUDIO         0    // Not yet connected
#define FEATURE_SENSORS       0    // Not yet connected
