#pragma once
// =============================================================================
// board_config.h — PocketOS Hardware Pin Definitions
// Waveshare ESP32-S3-Zero + ST7735S 128×128 TFT
//
// TFT SPI pins are defined via TFT_eSPI build flags in platformio.ini:
//   TFT_MOSI=11  TFT_SCLK=12  TFT_CS=10  TFT_DC=8  TFT_RST=9  TFT_BL=7
//
// All other pins are defined here. Every pin marked "TODO: VERIFY" must be
// confirmed against your physical wiring before flashing.
// See docs/WIRING.md for the full wiring diagram.
// =============================================================================

// ---------------------------------------------------------------------------
// SD Card  (shares SPI bus with TFT; independent chip-select)
// ---------------------------------------------------------------------------
// TODO: VERIFY — SD card chip-select GPIO
#define PIN_SD_CS           6

// ---------------------------------------------------------------------------
// Buttons  (active-LOW with internal pull-up)
// ---------------------------------------------------------------------------
// TODO: VERIFY — physical button GPIO assignments
#define PIN_BUTTON_UP       1
#define PIN_BUTTON_SELECT   2
#define PIN_BUTTON_DOWN     3

// Buttons connect GPIO to GND when pressed; MCU uses INPUT_PULLUP
#define BUTTON_ACTIVE_LOW   true

// ---------------------------------------------------------------------------
// Backlight PWM
// TFT_BL pin (7) is defined by TFT_eSPI build flags but might be undefined later.
// ---------------------------------------------------------------------------
#define PIN_BACKLIGHT       7       // Physical pin for backlight
#define BL_PWM_CHANNEL      0       // LEDC channel (0-7 on ESP32-S3)
#define BL_PWM_FREQ         1000    // Hz
#define BL_PWM_RESOLUTION   8       // bits — duty range 0-255
#define BL_PWM_MAX          255
#define BL_DEFAULT_BRIGHTNESS 180   // On-boot brightness (0-255)

// ---------------------------------------------------------------------------
// Shared SPI MISO  (used by SD; TFT is write-only and does not need MISO)
// ---------------------------------------------------------------------------
// TODO: VERIFY
#define PIN_SPI_MISO        13

// ---------------------------------------------------------------------------
// Battery ADC  (Future — not connected in Phase 1)
// ---------------------------------------------------------------------------
// TODO: VERIFY when battery circuit is added; -1 = not connected
#define PIN_BATTERY_ADC     -1

// ---------------------------------------------------------------------------
// I2S Audio  (Future — not connected in Phase 1)
// ---------------------------------------------------------------------------
// TODO: VERIFY when audio hardware is added
#define PIN_I2S_BCLK        -1
#define PIN_I2S_LRCK        -1
#define PIN_I2S_DATA        -1

// ---------------------------------------------------------------------------
// Display Geometry  (matches TFT_WIDTH / TFT_HEIGHT build flags)
// ---------------------------------------------------------------------------
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       128

// Status bar occupies the top 12 pixels
#define STATUS_BAR_HEIGHT   12

// Usable content area below the status bar
#define CONTENT_TOP         STATUS_BAR_HEIGHT
#define CONTENT_HEIGHT      (SCREEN_HEIGHT - STATUS_BAR_HEIGHT)

// ---------------------------------------------------------------------------
// Display rotation passed to tft.setRotation()
// 0 = portrait, 1 = landscape, 2 = portrait-flipped, 3 = landscape-flipped
// TODO: VERIFY correct orientation for your physical mounting
// ---------------------------------------------------------------------------
#define DISPLAY_ROTATION    0
