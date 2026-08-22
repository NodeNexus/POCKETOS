# PocketOS Applications Guide

## Creating a New Application

### 1. Create the source files

```
src/apps/myapp/
├── MyApp.h
└── MyApp.cpp
```

### 2. Implement the App interface

```cpp
// MyApp.h
#pragma once
#include "apps/App.h"

class MyApp : public App {
public:
    const AppInfo& getInfo()              const override;
    void onCreate()                             override;
    void onEnter()                              override;
    void update(uint32_t deltaMs)               override;
    void render(TFT_eSPI& tft)                  override;
    void handleInput(AppEvent event)            override;
    void onExit()                               override;

private:
    static const AppInfo _info;
    bool _needsRedraw = true;
};
```

### 3. Implement lifecycle methods

| Method | When called | Typical use |
|--------|-------------|-------------|
| `onCreate()` | Once, on first launch | Allocate resources |
| `onEnter()` | Each time app becomes active | Reset UI state, set `_needsRedraw = true` |
| `update(delta)` | Every loop frame | Advance logic, timers |
| `render(tft)` | Every render frame | Draw to display |
| `handleInput(event)` | For each queued button event | Handle navigation |
| `onExit()` | When leaving app | Save state, free temp resources |

### 4. Implement render efficiently

```cpp
void MyApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);  // Always — renders only if dirty
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    // ... draw your content ...
}
```

### 5. Draw an icon

Add an icon function following the `AppIcons.h` pattern:

```cpp
static void drawMyIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    // Draw a ~20×20 icon centred at (cx, cy)
    tft.drawCircle(cx, cy, 8, color);
}

const AppInfo MyApp::_info = { 10, "MyApp", drawMyIcon };
```

### 6. Register in main.cpp

```cpp
#include "apps/myapp/MyApp.h"
static MyApp myApp;

// In setup():
launcher.registerApp(&myApp);
```

---

## Navigation Conventions

| Action | Typical mapping |
|--------|----------------|
| Move up | `BTN_UP_SHORT` |
| Move down | `BTN_DOWN_SHORT` |
| Confirm/Open | `BTN_SELECT_SHORT` |
| Back (system default) | `BTN_SELECT_LONG` → AppManager pops stack |
| Home (system default) | `BTN_SELECT_VERY_LONG` → AppManager goes to launcher |

Apps can consume `BTN_SELECT_LONG` in `handleInput()` to override the default back behaviour (e.g., to show a confirmation dialog first).

---

## Using System Services

```cpp
// Display
TFT_eSPI& tft = DisplayManager::instance().tft();

// Settings
auto& s = SettingsManager::instance();
uint8_t brightness = s.getBrightness();

// Storage
StorageManager::instance().writeFile("/PocketOS/saves/myapp.json", content);

// Notifications
NotificationManager::instance().push("Saved!", NotifIcon::INFO);

// Time
char buf[8];
TimeManager::instance().getTimeString(buf, sizeof(buf));

// Network
if (NetworkManager::instance().isConnected()) { /* ... */ }
```

---

## Phase Roadmap

| Phase | Apps |
|-------|------|
| 2 | Files (SD browser, text viewer) |
| 3 | Network (scanner, info, RSSI graph), NTP |
| 4 | Tools (clock, stopwatch, timer, calculator, unit converter) |
| 5 | Games (Snake, Pong, Breakout, 2048) |
| 6 | Weather, Raspberry Pi monitor + companion server |
| 7 | Photos (BMP viewer, slideshow), Audio framework |
| 8 | Virtual Pet (Tamagotchi-style, persisted state) |
| 9 | Polish (animations, sprite buffering, power sleep) |
