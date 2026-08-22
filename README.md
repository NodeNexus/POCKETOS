# PocketOS

A modular, application-based embedded operating environment for the **Waveshare ESP32-S3-Zero** with a **ST7735S 128×128 TFT display**, three physical buttons, microSD storage, Wi-Fi, and a clean expandable architecture.

PocketOS is not a demo sketch — it is a structured C++ firmware that behaves like a tiny handheld computer with an application launcher, reusable system services, persistent settings, and a polished 128×128 graphical interface.

---

## Features (Phase 1 — Foundation)

- **Boot animation** — step-by-step initialization with visual ✓/✗ feedback  
- **3×3 icon launcher** — paged icon grid with animated selection  
- **Dark / Light / Cyber themes** — persisted in NVS flash  
- **Status bar** — live time, Wi-Fi status, SD indicator  
- **Settings app** — brightness, theme, Wi-Fi toggle, storage info, About  
- **Notification toasts** — slide-in/out overlays, non-blocking  
- **Power management** — dim → sleep after configurable inactivity  
- **SD card** — graceful detection and directory scaffolding  
- **Modular app stack** — launch/exit/home navigation via 3 buttons  

---

## Hardware

| Component | Part |
|-----------|------|
| MCU | Waveshare ESP32-S3-Zero |
| Display | ST7735S 128×128 TFT (SPI) |
| Input | 3 × push buttons (UP / SELECT / DOWN) |
| Storage | SPI microSD card module |
| Power | USB-C (battery circuit optional) |

> See [docs/WIRING.md](docs/WIRING.md) for the complete pin assignment table.

---

## Project Structure

```
PocketOS/
├── platformio.ini        ← Build config + TFT_eSPI pin flags
├── include/
│   ├── board_config.h    ← ALL GPIO assignments
│   ├── system_config.h   ← Timing constants, feature flags
│   └── debug.h           ← LOG_* macros
└── src/
    ├── main.cpp          ← setup() + loop()
    ├── core/             ← System services (Display, Input, Storage …)
    ├── ui/               ← Theme, Launcher, Menu, Dialog, Widgets, StatusBar
    └── apps/             ← App base class + all applications
        ├── App.h
        ├── AppIcons.h    ← Programmatic icon drawing
        ├── PlaceholderApp.h/.cpp
        └── settings/
            └── SettingsApp.h/.cpp
```

---

## Requirements

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)
- Python 3.8+

---

## Installation

```bash
git clone <your-repo-url> PocketOS
cd PocketOS
# Install PlatformIO if needed:
pip install platformio
```

---

## Building & Flashing

```bash
# Build only
pio run

# Build and flash
pio run --target upload

# Open serial monitor (115200 baud)
pio device monitor --baud 115200
```

> The Waveshare ESP32-S3-Zero uses USB CDC for serial output.  
> Make sure `-DARDUINO_USB_CDC_ON_BOOT=1` is set in `platformio.ini` (it is by default).

---

## ⚠️ Before Flashing — Verify Wiring

All GPIO numbers are in **`include/board_config.h`** and the TFT-specific pins are in **`platformio.ini`** build flags.

Every pin that requires physical verification is marked `// TODO: VERIFY`.

**You must confirm the correct GPIO assignments for your specific wiring before flashing.**

See [docs/WIRING.md](docs/WIRING.md).

---

## Library Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| `bodmer/TFT_eSPI` | ^2.5.43 | Display driver + sprites |
| `bblanchon/ArduinoJson` | ^7.2.1 | JSON config parsing (Phase 3+) |
| ESP32 Arduino (built-in) | — | WiFi, SD, Preferences, time |

---

## SD Card Directory Layout

PocketOS creates this structure on first mount:

```
/PocketOS/
├── config/     ← JSON configuration files
├── saves/      ← App save data (games, pet, etc.)
├── photos/     ← Photo viewer images
├── music/      ← Audio playback (Phase 7+)
├── logs/       ← Optional debug logs
└── cache/      ← Weather/API cache
```

---

## Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|--------------|-----|
| Black screen | Wrong TFT pins | Check `platformio.ini` TFT_* flags |
| Wrong colours | BGR vs RGB order | Uncomment `-DTFT_RGB_ORDER=TFT_BGR` |
| Inverted display | Inversion setting | Uncomment `-DTFT_INVERSION_ON=1` |
| SD not found | Wrong CS pin | Check `PIN_SD_CS` in `board_config.h` |
| Buttons unresponsive | Wrong GPIO | Check `PIN_BUTTON_*` in `board_config.h` |
| No serial output | USB CDC not configured | Check `-DARDUINO_USB_CDC_ON_BOOT=1` |

---

## Development Phases

| Phase | Status | Description |
|-------|--------|-------------|
| 1 — Foundation | ✅ Complete | Display, input, launcher, settings, boot |
| 2 — Storage | ⬜ Planned | SD file browser, text viewer, asset loading |
| 3 — Network | ⬜ Planned | Wi-Fi scan/connect, NTP, RSSI graph |
| 4 — Tools | ⬜ Planned | Clock, stopwatch, timer, calculator |
| 5 — Games | ⬜ Planned | Snake, Pong, Breakout, 2048 |
| 6 — Connected Apps | ⬜ Planned | Weather, Raspberry Pi monitor |
| 7 — Media | ⬜ Planned | Photo viewer, slideshow |
| 8 — Virtual Pet | ⬜ Planned | Tamagotchi-style pet |
| 9 — Polish | ⬜ Planned | Animations, power management, optimisation |

---

## Adding a New Application

1. Create `src/apps/myapp/MyApp.h` and `MyApp.cpp`
2. Inherit from `App` and implement the lifecycle methods
3. Add an icon drawing function following the `AppIcons.h` pattern
4. Instantiate statically in `main.cpp`
5. Call `launcher.registerApp(&myApp)` in `setup()`

See [docs/APPLICATIONS.md](docs/APPLICATIONS.md) for the full guide.

---

## Docs

- [Architecture](docs/ARCHITECTURE.md)
- [Wiring](docs/WIRING.md)
- [Applications](docs/APPLICATIONS.md)
