#include "src/apps/files/FilesApp.h"
#include "src/core/StorageManager.h"
#include "src/ui/StatusBar.h"
#include "src/ui/Theme.h"
#include "src/debug.h"
#include "src/system_config.h"
#include <string.h>

void FilesApp::drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color) {
    tft.fillRect(cx - 8, cy - 5, 7, 3, color);
    tft.drawRect(cx - 8, cy - 3, 16, 10, color);
    tft.drawFastHLine(cx - 5, cy + 1, 10, color);
    tft.drawFastHLine(cx - 5, cy + 4, 7,  color);
}

const AppInfo FilesApp::_info = { 4, "Files", FilesApp::drawIcon };

FilesApp::FilesApp() {}

void FilesApp::onCreate() {
    LOG_INFO("FilesApp: onCreate");
}

void FilesApp::onEnter() {
    _screen = FileScreen::BROWSER;
    _needsRedraw = true;
    
    // Default to PocketOS directory if it exists, else root
    if (StorageManager::instance().fileExists(SD_POCKETOS_DIR)) {
        strncpy(_currentPath, SD_POCKETOS_DIR, sizeof(_currentPath) - 1);
    } else {
        strncpy(_currentPath, "/", sizeof(_currentPath) - 1);
    }
    
    loadDirectory(_currentPath);
}

void FilesApp::onExit() {
    _viewer.close();
}

void FilesApp::loadDirectory(const char* path) {
    _menu.clear();
    
    if (!StorageManager::instance().isAvailable()) {
        _menu.addItem("No SD Card", "");
        _needsRedraw = true;
        return;
    }

    _fileCount = 0;
    StorageManager::instance().listDir(path, _files, 32, _fileCount);

    if (_fileCount == 0) {
        _menu.addItem("Empty Directory", "");
    } else {
        // Sort basic (directories first, then files)
        // A simple bubble sort for 32 items is fine
        for (int i = 0; i < _fileCount - 1; i++) {
            for (int j = 0; j < _fileCount - i - 1; j++) {
                bool swap = false;
                if (_files[j].isDirectory == _files[j+1].isDirectory) {
                    if (strcasecmp(_files[j].name, _files[j+1].name) > 0) swap = true;
                } else if (!_files[j].isDirectory && _files[j+1].isDirectory) {
                    swap = true;
                }
                
                if (swap) {
                    FileInfo temp = _files[j];
                    _files[j] = _files[j+1];
                    _files[j+1] = temp;
                }
            }
        }

        // Add to menu
        for (int i = 0; i < _fileCount; i++) {
            if (_files[i].isDirectory) {
                _menu.addItem(_files[i].name, "/");
            } else {
                char sizeStr[10];
                if (_files[i].size < 1024) snprintf(sizeStr, sizeof(sizeStr), "%lu B", (unsigned long)_files[i].size);
                else snprintf(sizeStr, sizeof(sizeStr), "%lu K", (unsigned long)(_files[i].size / 1024));
                _menu.addItem(_files[i].name, sizeStr);
            }
        }
    }
    
    _needsRedraw = true;
}

void FilesApp::enterDirectory(const char* name) {
    if (strcmp(_currentPath, "/") == 0) {
        snprintf(_currentPath, sizeof(_currentPath), "/%s", name);
    } else {
        char temp[64];
        snprintf(temp, sizeof(temp), "%s/%s", _currentPath, name);
        strncpy(_currentPath, temp, sizeof(_currentPath) - 1);
    }
    loadDirectory(_currentPath);
}

void FilesApp::goUpDirectory() {
    if (strcmp(_currentPath, "/") == 0) {
        requestExit = true; // Let AppManager exit the app
        return;
    }
    
    char* lastSlash = strrchr(_currentPath, '/');
    if (lastSlash != nullptr) {
        if (lastSlash == _currentPath) {
            strcpy(_currentPath, "/"); // Root
        } else {
            *lastSlash = '\0'; // Truncate at last slash
        }
        loadDirectory(_currentPath);
    }
}

void FilesApp::update(uint32_t /*deltaMs*/) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
}

void FilesApp::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    
    if (_screen == FileScreen::VIEWER) {
        _viewer.render(tft, _needsRedraw);
        _needsRedraw = false;
        return;
    }

    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    // Header bar with current path
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, 18, c.surface);
    tft.setTextColor(c.primary);
    
    // Truncate path if too long
    const char* dispPath = _currentPath;
    if (strlen(_currentPath) > 18) {
        dispPath = strrchr(_currentPath, '/');
        if (!dispPath) dispPath = _currentPath;
    }
    
    int16_t tw = tft.textWidth(dispPath, 1);
    tft.drawString(dispPath, (SCREEN_WIDTH - tw) / 2, STATUS_BAR_HEIGHT + 4, 1);
    tft.drawFastHLine(0, STATUS_BAR_HEIGHT + 17, SCREEN_WIDTH, c.border);

    // Render list
    _menu.render(tft, STATUS_BAR_HEIGHT + 19, CONTENT_HEIGHT - 19);
}

void FilesApp::handleInput(AppEvent e) {
    if (_screen == FileScreen::BROWSER) handleBrowserInput(e);
    else handleViewerInput(e);
}

void FilesApp::handleBrowserInput(AppEvent e) {
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
            if (_fileCount > 0 && StorageManager::instance().isAvailable()) {
                uint8_t sel = _menu.getSelected();
                if (_files[sel].isDirectory) {
                    enterDirectory(_files[sel].name);
                } else {
                    // Try to open it in viewer
                    char fullPath[80];
                    if (strcmp(_currentPath, "/") == 0) {
                        snprintf(fullPath, sizeof(fullPath), "/%s", _files[sel].name);
                    } else {
                        snprintf(fullPath, sizeof(fullPath), "%s/%s", _currentPath, _files[sel].name);
                    }
                    
                    if (_viewer.open(fullPath)) {
                        _screen = FileScreen::VIEWER;
                        _needsRedraw = true;
                    }
                }
            }
            break;
            
        case AppEvent::BTN_SELECT_LONG:
            goUpDirectory();
            break;
            
        default:
            break;
    }
}

void FilesApp::handleViewerInput(AppEvent e) {
    switch (e) {
        case AppEvent::BTN_UP_SHORT:
        case AppEvent::BTN_UP_HELD:
            _viewer.scrollUp();
            break;
            
        case AppEvent::BTN_DOWN_SHORT:
        case AppEvent::BTN_DOWN_HELD:
            _viewer.scrollDown();
            break;
            
        case AppEvent::BTN_SELECT_SHORT:
        case AppEvent::BTN_SELECT_LONG:
            // Close viewer and return to browser
            _viewer.close();
            _screen = FileScreen::BROWSER;
            _needsRedraw = true;
            break;
            
        default:
            break;
    }
}
