#include "src/apps/media/PhotosApp.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include "src/core/StorageManager.h"
#include <TJpg_Decoder.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h> // TJpg_Decoder might use standard SD

// Static pointer for TJpg_Decoder callback
static TFT_eSPI* currentTft = nullptr;

static bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (y >= SCREEN_HEIGHT) return 0;
    if (currentTft) {
        currentTft->pushImage(x, y, w, h, bitmap);
    }
    return 1;
}

void PhotosApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.drawRect(cx - 8, cy - 6, 16, 12, color);
    tft.fillTriangle(cx - 8, cy + 6, cx, cy - 2, cx + 4, cy + 6, color);
    tft.fillTriangle(cx + 2, cy + 6, cx + 5, cy + 2, cx + 8, cy + 6, color);
    tft.drawCircle(cx + 3, cy - 2, 2, color);
}

const AppInfo PhotosApp::_info = { 8, "Photos", PhotosApp::drawIcon };

PhotosApp::PhotosApp() {}

void PhotosApp::onCreate() {
    LOG_INFO("PhotosApp: onCreate");
    TJpgDec.setJpgScale(1);
    TJpgDec.setSwapBytes(true); // TFT_eSPI usually needs swapped bytes
    TJpgDec.setCallback(tft_output);
}

void PhotosApp::onEnter() {
    _screen = PhotoScreen::BROWSER;
    _needsRedraw = true;
    _currentImageIndex = -1;
    loadDirectory();
}

void PhotosApp::onExit() {
}

void PhotosApp::loadDirectory() {
    _menu.clear();
    
    if (!StorageManager::instance().isAvailable()) {
        _menu.addItem("No SD Card", "");
        _needsRedraw = true;
        return;
    }

    _fileCount = 0;
    StorageManager::instance().listDir("/PocketOS/photos", _files, 32, _fileCount);

    int imgCount = 0;
    for (int i = 0; i < _fileCount; i++) {
        if (!_files[i].isDirectory) {
            String name = _files[i].name;
            name.toLowerCase();
            if (name.endsWith(".jpg") || name.endsWith(".jpeg")) {
                _menu.addItem(_files[i].name, "");
                imgCount++;
            }
        }
    }
    
    if (imgCount == 0) {
        _menu.addItem("No JPGs found", "");
    }
    _needsRedraw = true;
}

void PhotosApp::showImage(int index) {
    if (index < 0 || index >= _fileCount) return;
    _currentImageIndex = index;
    _needsRedraw = true;
}

void PhotosApp::update(uint32_t deltaMs) {
    if (_screen == PhotoScreen::BROWSER) {
        StatusBar::instance().update();
        if (StatusBar::instance().isDirty()) _needsRedraw = true;
    } else if (_screen == PhotoScreen::SLIDESHOW) {
        _slideshowTimerMs += deltaMs;
        if (_slideshowTimerMs > 3000) { // 3 second delay
            _slideshowTimerMs = 0;
            if (_menu.getCount() > 0 && _fileCount > 0) {
                int nextIdx = _currentImageIndex + 1;
                if (nextIdx >= _menu.getCount()) nextIdx = 0;
                showImage(nextIdx);
                _menu.setSelected(nextIdx);
            }
        }
    }
}

void PhotosApp::render(TFT_eSPI& tft) {
    if (_screen == PhotoScreen::BROWSER) {
        StatusBar::instance().render(tft);
        
        if (!_needsRedraw) return;
        _needsRedraw = false;

        const ThemeColors& c = Theme::instance().colors();
        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

        tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
        tft.setTextColor(c.primary);
        int16_t tw = tft.textWidth("Photos", 1);
        tft.drawString("Photos", (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 4, 1);
        tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);

        _menu.render(tft, STATUS_BAR_HEIGHT + 19, CONTENT_HEIGHT - 19);
    } else if (_screen == PhotoScreen::VIEWER || _screen == PhotoScreen::SLIDESHOW) {
        if (!_needsRedraw) return;
        _needsRedraw = false;
        
        const ThemeColors& c = Theme::instance().colors();
        tft.fillScreen(c.background);
        
        if (_currentImageIndex >= 0 && _currentImageIndex < _fileCount) {
            String fullPath = String("/PocketOS/photos/") + _files[_currentImageIndex].name;
            currentTft = &tft;
            
            // Get dimensions
            uint16_t w = 0, h = 0;
            TJpgDec.getSdJpgSize(&w, &h, fullPath.c_str());
            
            // Center the image if smaller than screen
            int16_t x = 0;
            int16_t y = 0;
            if (w > 0 && w < SCREEN_WIDTH) x = (SCREEN_WIDTH - w) / 2;
            if (h > 0 && h < SCREEN_HEIGHT) y = (SCREEN_HEIGHT - h) / 2;
            
            TJpgDec.drawSdJpg(x, y, fullPath.c_str());
            currentTft = nullptr;
        } else {
            tft.setTextColor(c.error);
            tft.drawString("Image not found", 10, 60, 1);
        }
    }
}

void PhotosApp::handleInput(AppEvent e) {
    if (_screen == PhotoScreen::BROWSER) {
        switch (e) {
            case AppEvent::BTN_UP_SHORT:
            case AppEvent::BTN_UP_HELD:
                _menu.selectPrev();
                _needsRedraw = true;
                break;
            case AppEvent::BTN_DOWN_SHORT:
            case AppEvent::BTN_DOWN_HELD:
                _menu.selectNext();
                _needsRedraw = true;
                break;
            case AppEvent::BTN_SELECT_SHORT:
                if (_fileCount > 0 && String(_files[_menu.getSelected()].name).endsWith("g")) {
                    _screen = PhotoScreen::VIEWER;
                    showImage(_menu.getSelected());
                }
                break;
            case AppEvent::BTN_SELECT_LONG:
                requestExit = true;
                break;
            default: break;
        }
    } else if (_screen == PhotoScreen::VIEWER) {
        switch (e) {
            case AppEvent::BTN_SELECT_SHORT:
                _screen = PhotoScreen::SLIDESHOW;
                _slideshowTimerMs = 0;
                break;
            case AppEvent::BTN_SELECT_LONG:
                _screen = PhotoScreen::BROWSER;
                _needsRedraw = true;
                break;
            case AppEvent::BTN_UP_SHORT:
                if (_currentImageIndex > 0) showImage(_currentImageIndex - 1);
                _menu.setSelected(_currentImageIndex);
                break;
            case AppEvent::BTN_DOWN_SHORT:
                if (_currentImageIndex < _fileCount - 1) showImage(_currentImageIndex + 1);
                _menu.setSelected(_currentImageIndex);
                break;
            default: break;
        }
    } else if (_screen == PhotoScreen::SLIDESHOW) {
        if (e == AppEvent::BTN_SELECT_SHORT || e == AppEvent::BTN_SELECT_LONG) {
            _screen = PhotoScreen::VIEWER;
            _needsRedraw = true;
        }
    }
}
