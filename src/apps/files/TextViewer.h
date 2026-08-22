#pragma once
// =============================================================================
// TextViewer.h — Memory-safe text file viewer
//
// Streams text line-by-line from the SD card. Supports scrolling up and down.
// Caches file offsets for fast scrolling without holding the entire file in RAM.
// =============================================================================

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "src/core/StorageManager.h"

class TextViewer {
public:
    TextViewer() = default;

    // Open a file for viewing (must be a valid text file)
    bool open(const char* path);
    void close();

    void update(uint32_t deltaMs) {}
    void render(TFT_eSPI& tft, bool forceRedraw);

    // Scroll up/down by lines
    void scrollUp();
    void scrollDown();

    bool isOpen() const { return _file; }

    static constexpr uint8_t LINES_PER_PAGE = 10;
    static constexpr uint8_t CHARS_PER_LINE = 21; // 128 / 6 (font 1 width) = 21.3

private:
    void readCurrentPage();
    void buildLineCache();

    File     _file;
    char     _path[64] = "";
    
    char     _lineBuf[LINES_PER_PAGE][CHARS_PER_LINE + 1];
    uint32_t _lineOffsets[256]; // Cache up to 256 lines (sufficient for basic files)
    
    uint16_t _totalLines   = 0;
    uint16_t _currentLine  = 0; // The top visible line
    bool     _needsRead    = true;
};
