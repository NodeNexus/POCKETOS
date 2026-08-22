# PocketOS — Arduino IDE Quick Start

## 1. Install Board Package
Add this URL in File → Preferences → Additional Boards Manager URLs:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Then install: Tools → Boards Manager → "esp32 by Espressif Systems"

## 2. Install Libraries (Tools → Manage Libraries)
- TFT_eSPI           (by Bodmer)
- ArduinoJson        (by Benoit Blanchon)  ← version 7.x
- TJpg_Decoder       (by Bodmer)

## 3. Configure TFT_eSPI (REQUIRED)
Copy User_Setup.h from this sketch folder into your TFT_eSPI library:
  Documents\Arduino\libraries\TFT_eSPI\User_Setup.h   (overwrite existing)

Also open TFT_eSPI\User_Setup_Select.h and comment out all #include lines.

## 4. Board Settings (Tools menu)
  Board:                ESP32S3 Dev Module
  USB CDC On Boot:      Enabled
  PSRAM:                OPI PSRAM
  Partition Scheme:     Default 4MB with spiffs
  CPU Frequency:        240MHz
  Upload Speed:         921600

## 5. Open & Compile
  File → Open → POCKETOS.ino
  Sketch → Verify/Compile

## Pin Summary
  GPIO 11  TFT MOSI (shared SPI)
  GPIO 12  TFT SCLK (shared SPI)
  GPIO 10  TFT CS
  GPIO  8  TFT DC
  GPIO  9  TFT RST
  GPIO  7  TFT Backlight (PWM)
  GPIO  6  SD Card CS     ← verify against your wiring
  GPIO 13  SD MISO        ← verify against your wiring
  GPIO  1  Button UP      (active LOW, pulled up)
  GPIO  2  Button SELECT  (active LOW, pulled up)
  GPIO  3  Button DOWN    (active LOW, pulled up)

## Notes
- All original PocketOS logic is fully preserved.
- platformio.ini is no longer needed (can be deleted or ignored).
- The src/ folder structure is supported natively by Arduino IDE.
