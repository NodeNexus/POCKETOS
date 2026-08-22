# PocketOS Wiring Guide

> ⚠️ **All GPIO numbers marked "TODO: VERIFY" must be confirmed against your actual wiring before flashing.**

---

## TFT Display — ST7735S 128×128

TFT pins are defined as **TFT_eSPI build flags** in `platformio.ini`.  
Edit those values to match your wiring.

| Signal | ESP32-S3-Zero GPIO | Notes |
|--------|-------------------|-------|
| MOSI (SDA) | **11** | Shared SPI bus |
| SCK (SCL)  | **12** | Shared SPI bus |
| CS         | **10** | TFT chip select |
| DC (RS)    | **8**  | Data/Command |
| RST        | **9**  | Reset (or tie to 3.3V + software reset) |
| BL         | **7**  | Backlight PWM via LEDC |
| VCC        | 3.3V  | |
| GND        | GND   | |

> **To change TFT pins**: edit the `-DTFT_MOSI=xx` etc. lines in `platformio.ini`.

---

## SD Card Module

SD pins are defined in `include/board_config.h`.

| Signal | ESP32-S3-Zero GPIO | Notes |
|--------|-------------------|-------|
| MOSI   | **11** | Shared with TFT |
| SCK    | **12** | Shared with TFT |
| MISO   | **13** | SD only (TFT is write-only) |
| CS     | **6**  | SD chip select — `PIN_SD_CS` |
| VCC    | 3.3V  | |
| GND    | GND   | |

> `PIN_SD_CS` in `board_config.h` must match your wiring.

---

## Buttons

All buttons connect their GPIO to **GND** when pressed.  
The MCU uses `INPUT_PULLUP` — no external resistors required.

| Button | ESP32-S3-Zero GPIO | `board_config.h` define |
|--------|-------------------|------------------------|
| UP     | **1** | `PIN_BUTTON_UP` |
| SELECT | **2** | `PIN_BUTTON_SELECT` |
| DOWN   | **3** | `PIN_BUTTON_DOWN` |

---

## Future Hardware Pinouts (Reserved)

These are placeholders. Connect hardware and update `board_config.h`.

| Hardware | Signal | Suggested GPIO |
|----------|--------|---------------|
| Battery ADC | Voltage divider | 4 |
| I2S DAC BCLK | — | 5 |
| I2S DAC LRCK | — | 6 (may conflict with SD_CS) |
| I2S DAC DATA | — | 14 |
| I2C SDA (sensors) | — | 15 |
| I2C SCL (sensors) | — | 16 |

---

## Power

- Supply: USB-C (5V → onboard 3.3V LDO)
- LiPo battery: connect via MCP73831 charger circuit (Phase 9)
- Battery ADC: voltage divider from battery+ → GPIO → GND

---

## Schematic Notes

- **TFT_BL** can be connected directly to 3.3V for constant-on backlight (bypasses PWM dimming)
- **TFT_RST** can be tied to the ESP32 EN pin if you don't need software reset
- The SD card module's 3.3V regulator should be enabled (most modules have one)
- All grounds must share a common reference

---

## Waveshare ESP32-S3-Zero Pinout Reference

The Waveshare ESP32-S3-Zero has two rows of pins. GPIO numbers 0-21 are generally available. Refer to the official datasheet at:

`https://www.waveshare.com/wiki/ESP32-S3-Zero`

for the definitive pinout diagram before finalising your wiring.
