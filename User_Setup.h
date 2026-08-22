// =============================================================================
// User_Setup.h  —  TFT_eSPI configuration for PocketOS
// Hardware: Waveshare ESP32-S3-Zero + ST7735S 128x128 BLUE PCB (1.44")
// =============================================================================

// Driver
#define ST7735_DRIVER

// Blue PCB 128x128 (1.44") uses INITR_144GREENTAB
// If still white, try INITR_GREENTAB2 next
// #define INITR_144GREENTAB
// #define INITR_GREENTAB2
#define INITR_BLACKTAB

// Display dimensions
#define TFT_WIDTH   128
#define TFT_HEIGHT  128

// SPI pins — Waveshare ESP32-S3-Zero
#define TFT_MOSI     8 // SDA
#define TFT_SCLK     7 // SCL
#define TFT_CS      11
#define TFT_DC      10
#define TFT_RST      9
#define TFT_BL      12 // BLK

// SPI frequency
#define SPI_FREQUENCY       27000000
#define SPI_READ_FREQUENCY   5000000

// Fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT
