#include "src/apps/files/TextViewer.h"
#include "src/core/StorageManager.h"
#include "src/ui/Theme.h"
#include "src/ui/StatusBar.h"
#include "src/board_config.h"

bool TextViewer::open(const char* path) {
    close();
    _file = StorageManager::instance().openFile(path);
    if (!_file) return false;

    strncpy(_path, path, sizeof(_path) - 1);
    _currentLine = 0;
    _needsRead   = true;
    
    buildLineCache();
    return true;
}

void TextViewer::close() {
    if (_file) {
        _file.close();
    }
    _path[0] = '\0';
}

void TextViewer::buildLineCache() {
    _totalLines = 0;
    if (!_file) return;

    _file.seek(0);
    _lineOffsets[0] = 0;
    
    uint32_t offset = 0;
    while (_file.available() && _totalLines < 255) {
        char c = _file.read();
        offset++;
        if (c == '\n') {
            _totalLines++;
            _lineOffsets[_totalLines] = offset;
        }
    }
    // If file doesn't end with newline, count the last line
    if (offset > 0 && _lineOffsets[_totalLines] != offset) {
        _totalLines++;
    }
}

void TextViewer::readCurrentPage() {
    if (!_file || !_needsRead) return;
    
    // Clear buffer
    for (int i = 0; i < LINES_PER_PAGE; i++) {
        _lineBuf[i][0] = '\0';
    }

    if (_currentLine >= _totalLines) return;

    _file.seek(_lineOffsets[_currentLine]);
    
    for (int i = 0; i < LINES_PER_PAGE && _file.available(); i++) {
        int charIdx = 0;
        while (_file.available() && charIdx < CHARS_PER_LINE) {
            char c = _file.read();
            if (c == '\r') continue;
            if (c == '\n') break;
            // Simple tab to space conversion
            if (c == '\t') c = ' ';
            _lineBuf[i][charIdx++] = c;
        }
        _lineBuf[i][charIdx] = '\0';
        
        // If we hit the char limit, consume rest of line to avoid wrapping issues for now
        // (A more advanced viewer would wrap, but we'll just truncate for simplicity)
        if (charIdx == CHARS_PER_LINE) {
            while (_file.available()) {
                char c = _file.read();
                if (c == '\n') break;
            }
        }
    }
    _needsRead = false;
}

void TextViewer::scrollUp() {
    if (_currentLine > 0) {
        _currentLine--;
        _needsRead = true;
    }
}

void TextViewer::scrollDown() {
    if (_totalLines > LINES_PER_PAGE && _currentLine < _totalLines - LINES_PER_PAGE) {
        _currentLine++;
        _needsRead = true;
    }
}

void TextViewer::render(TFT_eSPI& tft, bool forceRedraw) {
    if (!forceRedraw && !_needsRead) return; // Only redraw if scroll happened or forced
    
    if (_needsRead) readCurrentPage();

    const ThemeColors& c = Theme::instance().colors();
    
    // Content background
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    // Header (file name)
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 14, c.surface);
    tft.setTextColor(c.primary);
    
    // Just show base name
    const char* baseName = strrchr(_path, '/');
    baseName = baseName ? baseName + 1 : _path;
    tft.drawString(baseName, 4, STATUS_BAR_HEIGHT + 2, 1);
    
    // Line indicator
    char ind[16];
    snprintf(ind, sizeof(ind), "%d/%d", _currentLine + 1, _totalLines);
    tft.setTextColor(c.textMuted);
    tft.drawString(ind, SCREEN_WIDTH - tft.textWidth(ind, 1) - 4, STATUS_BAR_HEIGHT + 2, 1);
    
    tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 13, SCREEN_WIDTH, c.border);

    // Text area
    tft.setTextColor(c.text);
    int16_t y = STATUS_BAR_HEIGHT + 16;
    for (int i = 0; i < LINES_PER_PAGE; i++) {
        if (_lineBuf[i][0] != '\0') {
            tft.drawString(_lineBuf[i], 2, y, 1);
        }
        y += 9;
    }
}
