# PocketOS Architecture

## Design Principles

1. **No monolithic main file** — code is split by responsibility
2. **Cooperative scheduling** — `millis()`-based state machines, no `delay()` in the main loop
3. **Graceful degradation** — SD missing, Wi-Fi down, or API unreachable must not freeze the OS
4. **Static allocation** — apps are statically declared to avoid heap fragmentation
5. **Singleton services** — core services use `instance()` singletons for clean global access without scattered globals

---

## Boot Sequence

```
Serial.begin()
    │
DisplayManager::init()   ← TFT_eSPI init, backlight PWM
    │
Theme::init()            ← Load dark theme for boot
    │
Boot logo + fade in
    │
InputManager::init()     ← Configure button GPIO + debounce
SettingsManager::init()  ← Load NVS; apply saved theme + brightness
StorageManager::init()   ← Mount SD; create /PocketOS/ dirs
NetworkManager::init()   ← Init WiFi hardware (no connect yet)
TimeManager::init()      ← Set TZ; NTP deferred to Phase 3
NotificationManager::init()
StatusBar::init()
PowerManager::init()
    │
Register apps with Launcher
    │
AppManager::init(&launcher)  ← Launcher becomes stack base
    │
loop()
```

---

## Main Loop (cooperative)

```
InputManager::update()       // Poll buttons; fill event queue
PowerManager::reportActivity()  // Reset inactivity timer on input
AppManager::update(delta)    // Dispatch events → update → check flags
DisplayManager::update()     // Advance brightness fades
PowerManager::update()       // ACTIVE → DIMMED → SLEEP state machine
NotificationManager::update() // Advance toast animations
NetworkManager::update()     // Detect WiFi drops

// Render phase (≤30 FPS):
App::render(tft)
NotificationManager::render(tft)

// Periodic (30s):
SettingsManager::save()      // Only if dirty
```

---

## App Lifecycle

```
registerApp(&app)    ← Launcher adds to grid
    │
launchApp(&app)
    ├── prev->onExit()
    ├── app->onCreate()
    ├── app->onEnter()
    └── [update / render loop]
         │
         BTN_SELECT_LONG caught by AppManager
         │
exitCurrentApp()
    ├── app->onExit()
    └── prev->onEnter()
```

### Navigation Flags

Apps set `requestExit = true` or `requestHome = true` on their `App` base to request navigation. AppManager checks these after each `update()` call.

### System Navigation Interception

`BTN_SELECT_LONG` and `BTN_SELECT_VERY_LONG` are intercepted by AppManager before reaching the app when stack depth > 1. Apps that need to suppress this (e.g. a text entry dialog) must consume the event in `handleInput()` and not call the base class.

---

## Service Dependency Graph

```
main.cpp
  ├── DisplayManager    (no deps)
  ├── InputManager      (no deps)
  ├── SettingsManager   (Preferences/NVS)
  ├── StorageManager    (SD)
  ├── NetworkManager    (WiFi)
  ├── TimeManager       (time.h, NTP)
  ├── NotificationManager  (TFT_eSPI, Theme)
  ├── PowerManager      (DisplayManager)
  ├── AssetManager      (StorageManager)
  ├── StatusBar         (Time, Network, Storage)
  ├── Theme             (no deps)
  ├── Launcher          (AppManager, Theme, StatusBar)
  └── AppManager        (InputManager, App stack)
```

---

## SPI Bus Sharing

TFT and SD share the SPI bus. Conflicts are avoided by design:
- TFT writes happen exclusively in the **render phase**
- SD access happens in **update/background phases** (never concurrently with render)
- Each device has its own CS pin; neither is ever selected simultaneously

> Phase 9: A FreeRTOS SPI mutex will be added if background tasks are introduced.

---

## Memory Strategy

| Storage | Use |
|---------|-----|
| Flash (NVS) | Small critical settings (theme, brightness, credentials) |
| SD FAT | Large data (saves, photos, music, logs) |
| SRAM heap | Temporary buffers (UI sprites in Phase 9) |
| PSRAM (if present) | Large frame buffers, decoded images |

Write rules:
- **NVS**: saved only when `_dirty == true` (max every 30 s in main loop)
- **SD**: saved only on explicit user action or safe shutdown
- **Never write to flash every frame**

---

## Adding Future Hardware

All optional hardware is pre-stubbed:
- **Battery**: `PowerManager::getBatteryPercent()` returns 100 (stub)
- **Audio**: `AudioManager` (Phase 7) will use I2S DAC
- **Sensors**: `SensorManager` (Phase 8) will query I2C devices
- **RTC**: `TimeManager` will prefer RTC over NTP when available

Enable hardware by setting `FEATURE_*` flags in `system_config.h`.
