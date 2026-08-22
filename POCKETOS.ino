// =============================================================================
// POCKETOS.ino — PocketOS Entry Point  (Arduino IDE compatible)
//
// Board  : Waveshare ESP32-S3-Zero  (or any ESP32-S3 board)
// Select : "ESP32S3 Dev Module"  in Arduino IDE → Tools → Board
//
// Required libraries (install via Library Manager):
//   • TFT_eSPI        (by Bodmer)
//   • ArduinoJson     (by Benoit Blanchon)
//   • TJpg_Decoder    (by Bodmer)
//
// TFT_eSPI MUST be configured via User_Setup.h before compiling.
// See User_Setup.h in this sketch folder for pin definitions.
// Copy User_Setup.h into your TFT_eSPI library folder (overwrite existing).
//
// Boot sequence:
//   1. Serial init
//   2. Display init + boot animation
//   3. Core services init (shown on display)
//   4. App registration
//   5. Launcher
//
// Main loop: cooperative scheduling via millis().
//   InputManager → PowerManager → AppManager → DisplayManager → render → toast
// =============================================================================

#define POCKETOS_DEBUG 1
#define POCKETOS_VERSION_MAJOR 0
#define POCKETOS_VERSION_MINOR 1
#define POCKETOS_VERSION_PATCH 0

// NOTE: Arduino IDE automatically includes Arduino.h for .ino files.
// We still include it here so this file can be compiled as either .ino or .cpp.
#include <Arduino.h>
#include <TFT_eSPI.h>
#include <ArduinoJson.h>
#include <TJpg_Decoder.h>

// Config & Utilities
#include "src/board_config.h"
#include "src/system_config.h"
#include "src/debug.h"

// Core services
#include "src/core/DisplayManager.h"
#include "src/core/InputManager.h"
#include "src/core/SettingsManager.h"
#include "src/core/StorageManager.h"
#include "src/core/SysNetworkManager.h"
#include "src/core/PowerManager.h"
#include "src/core/NotificationManager.h"
#include "src/core/TimeManager.h"
#include "src/core/AssetManager.h"
#include "src/core/AppManager.h"

// UI
#include "src/ui/Theme.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Launcher.h"

// App icons
#include "src/apps/AppIcons.h"

// Apps
#include "src/apps/settings/SettingsApp.h"
#include "src/apps/files/FilesApp.h"
#include "src/apps/network/NetworkApp.h"
#include "src/apps/tools/ToolsApp.h"
#include "src/apps/games/GamesApp.h"
#include "src/apps/weather/WeatherApp.h"
#include "src/apps/monitor/MonitorApp.h"
#include "src/apps/media/PhotosApp.h"
#include "src/apps/pet/PetApp.h"

// =============================================================================
// App instances — static lifetime avoids heap fragmentation
// =============================================================================
static Launcher       launcher;

// Registered apps
static GamesApp       gamesApp;
static NetworkApp     networkApp;
static MonitorApp     monitorApp;
static FilesApp       filesApp;
static ToolsApp       toolsApp;
static WeatherApp     weatherApp;
static PetApp         petApp;
static PhotosApp      photosApp;
static SettingsApp    settingsApp;

// =============================================================================
// Boot screen helpers
// =============================================================================

static void bootLogo(TFT_eSPI& tft, int16_t yOffset) {
    const ThemeColors& c = Theme::instance().colors();
    tft.fillScreen(c.background);

    // "POCKET" in primary colour, large
    tft.setTextColor(c.primary);
    int16_t pw = tft.textWidth("POCKET", 4);
    tft.drawString("POCKET", (SCREEN_WIDTH - pw) / 2, 20 + yOffset, 4);

    // "OS" accent below
    tft.setTextColor(c.text);
    int16_t ow = tft.textWidth("OS", 2);
    tft.drawString("OS", (SCREEN_WIDTH - ow) / 2, 52 + yOffset, 2);

    // Thin accent line under the logo
    tft.drawFastHLine(20, 68 + yOffset, SCREEN_WIDTH - 40, c.primary);

    // Version
    char ver[20];
    snprintf(ver, sizeof(ver), "v%d.%d.%d",
             POCKETOS_VERSION_MAJOR, POCKETOS_VERSION_MINOR, POCKETOS_VERSION_PATCH);
    tft.setTextColor(c.textMuted);
    int16_t vw = tft.textWidth(ver, 1);
    tft.drawString(ver, (SCREEN_WIDTH - vw) / 2, 73 + yOffset, 1);
}

// Graphical loading bar
static void updateBootProgress(TFT_eSPI& tft, int step, int totalSteps, const char* label) {
    const ThemeColors& c = Theme::instance().colors();
    
    // Clear previous label
    tft.fillRect(0, 95, SCREEN_WIDTH, 12, c.background);
    
    tft.setTextColor(c.textMuted);
    int16_t lw = tft.textWidth(label, 1);
    tft.drawString(label, (SCREEN_WIDTH - lw) / 2, 95, 1);
    
    // Draw bar outline
    int16_t barW = 100;
    int16_t barH = 6;
    int16_t bx = (SCREEN_WIDTH - barW) / 2;
    int16_t by = 110;
    
    tft.drawRect(bx, by, barW, barH, c.border);
    
    // Fill progress
    int16_t fillW = (barW - 2) * step / totalSteps;
    tft.fillRect(bx + 1, by + 1, fillW, barH - 2, c.primary);
}

// =============================================================================
// setup()
// =============================================================================
void setup() {
    Serial.begin(115200);
    uint32_t boot = millis();

    LOG_INFO("=== PocketOS v%d.%d.%d booting ===",
             POCKETOS_VERSION_MAJOR, POCKETOS_VERSION_MINOR, POCKETOS_VERSION_PATCH);

    // ---- 1. Display --------------------------------------------------------
    DisplayManager& disp = DisplayManager::instance();
    disp.init();

    // ---- 2. Theme & Boot Logo ----------------------------------------------
    // Default theme during boot
    Theme::instance().init(ThemeId::DARK);

    // Fade backlight in while showing logo animating down
    int16_t yOff = -30;
    bootLogo(disp.tft(), yOff);
    disp.fadeToBrightness(BL_DEFAULT_BRIGHTNESS, 600);

    // Wait for fade to complete (blocking during boot only)
    uint32_t fadeStart = millis();
    while (millis() - fadeStart < 650) {
        if (yOff < 0) {
            yOff += 2;
            bootLogo(disp.tft(), yOff);
        }
        disp.update();
        delay(15);
    }
    bootLogo(disp.tft(), 0);

    // ---- 3. Init sequence with graphical progress --------------------------
    int totalSteps = 5;

    // 1. Input
    updateBootProgress(disp.tft(), 1, totalSteps, "Initializing Input");
    InputManager::instance().init();
    delay(BOOT_STEP_DELAY_MS);

    // 2. Settings
    updateBootProgress(disp.tft(), 2, totalSteps, "Loading Settings");
    bool settingsOK = SettingsManager::instance().init();
    (void)settingsOK; // suppress unused-variable warning

    // Apply persisted theme immediately
    Theme::instance().init(SettingsManager::instance().getTheme());

    // Rebuild logo with correct theme (theme may have changed from NVS)
    bootLogo(disp.tft(), 0);
    updateBootProgress(disp.tft(), 2, totalSteps, "Settings Loaded");
    delay(BOOT_STEP_DELAY_MS);

    // 3. Storage
    updateBootProgress(disp.tft(), 3, totalSteps, "Mounting SD Card");
    bool sdOK = StorageManager::instance().init();
    delay(BOOT_STEP_DELAY_MS);

    // 4. Network (only init hardware; don't connect yet)
    updateBootProgress(disp.tft(), 4, totalSteps, "Starting Network");
    SysNetworkManager::instance().init(SettingsManager::instance().getWifiEnabled());
    delay(BOOT_STEP_DELAY_MS);

    // 5. Time
    updateBootProgress(disp.tft(), 5, totalSteps, "Syncing Time");
    TimeManager::instance().init(SettingsManager::instance().getTimezone().c_str());
    delay(BOOT_STEP_DELAY_MS);

    // Post-init managers
    NotificationManager::instance().init();
    AssetManager::instance().init();
    StatusBar::instance().init();

    // Apply persisted brightness
    disp.setBrightness(SettingsManager::instance().getBrightness());

    // Power manager
    PowerManager::instance().init(
        POWER_DIM_TIMEOUT_MS,
        SettingsManager::instance().getSleepTimeout()
    );

    // ---- 4. Register apps with launcher ------------------------------------
    launcher.registerApp(&gamesApp);
    launcher.registerApp(&networkApp);
    launcher.registerApp(&monitorApp);
    launcher.registerApp(&filesApp);
    launcher.registerApp(&toolsApp);
    launcher.registerApp(&weatherApp);
    launcher.registerApp(&petApp);
    launcher.registerApp(&photosApp);
    launcher.registerApp(&settingsApp);

    // ---- 5. Launch --------------------------------------------------------
    delay(BOOT_FINISH_PAUSE_MS);

    // Wipe transition: fill screen with background colour line by line
    {
        const ThemeColors& c = Theme::instance().colors();
        for (int16_t y = 0; y < SCREEN_HEIGHT; y += 4) {
            disp.tft().fillRect(0, y, SCREEN_WIDTH, 4, c.background);
            delay(3);
        }
    }

    AppManager::instance().init(&launcher);

    // Post-boot notification if SD was missing
    if (!sdOK) {
        NotificationManager::instance().push("SD card not found",
                                             NotifIcon::SD_ERROR, 4000);
    }

    LOG_INFO("Boot complete in %lu ms", (unsigned long)(millis() - boot));
}

// =============================================================================
// loop()
// =============================================================================
static uint32_t lastMs       = 0;
static uint32_t lastRenderMs = 0;

void loop() {
    uint32_t now   = millis();
    uint32_t delta = now - lastMs;
    lastMs = now;

    // 1. Poll buttons
    InputManager::instance().update();

    // 2. Report activity to power manager on any button event
    if (InputManager::instance().hasEvent()) {
        PowerManager::instance().reportActivity();
    }

    // 3. App update (dispatches input, calls current app update)
    AppManager::instance().update(delta);

    // 4. Non-blocking brightness fade
    DisplayManager::instance().update();

    // 5. Power state machine (dim / sleep)
    PowerManager::instance().update();

    // 6. Notification advance
    NotificationManager::instance().update();

    // 7. Network watchdog
    SysNetworkManager::instance().update();

    // 8. Render phase — throttled to DISPLAY_TARGET_FPS
    if (now - lastRenderMs >= DISPLAY_FRAME_MS) {
        lastRenderMs = now;

        // Don't render while sleeping (display is off)
        if (!PowerManager::instance().isSleeping()) {
            TFT_eSPI& tft = DisplayManager::instance().tft();
            App* app = AppManager::instance().currentApp();
            if (app) app->render(tft);
            NotificationManager::instance().render(tft);
        }
    }

    // 9. Periodic settings save (every 30 s if dirty)
    static uint32_t lastSaveMs = 0;
    if (now - lastSaveMs > 30000) {
        lastSaveMs = now;
        SettingsManager::instance().save();
    }
}
