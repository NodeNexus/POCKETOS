#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>

// This is a minimal test sketch to verify the ST7735S display.
// If the screen is still white, your POCKETOS/User_Setup.h mentions trying INITR_GREENTAB2 instead of INITR_144GREENTAB.

TFT_eSPI tft = TFT_eSPI(); 

void setup(void) {
  Serial.begin(115200);
  Serial.println("ST7735S TFT Test Booting...");

  // Force backlight ON (if it's connected to pin 12 as per your User_Setup.h)
  int blPin = 12; 
  pinMode(blPin, OUTPUT);
  digitalWrite(blPin, HIGH);

  // Initialize the TFT display
  tft.init();
  tft.setRotation(0);
  
  // Fill screen with black to clear the white
  tft.fillScreen(TFT_BLACK);
  
  // Draw some test text
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Hello World!", 10, 10, 2);
  tft.drawString("ST7735S Test", 10, 30, 2);
  
  // Draw some basic colors
  tft.fillRect(10, 60, 20, 20, TFT_RED);
  tft.fillRect(40, 60, 20, 20, TFT_GREEN);
  tft.fillRect(70, 60, 20, 20, TFT_BLUE);
  
  Serial.println("Setup done. Screen should show colors now.");
}

void loop() {
  // Blink a small rectangle to show it's alive
  tft.fillRect(10, 90, 20, 20, TFT_YELLOW);
  delay(1000);
  tft.fillRect(10, 90, 20, 20, TFT_BLACK);
  delay(1000);
}
