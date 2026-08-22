#pragma once
// =============================================================================
// FilesApp.h — SD Card File Browser
//
// Browses the SD card filesystem. Supports entering directories and viewing
// basic text files.
// =============================================================================

#include "src/apps/App.h"
#include "src/ui/Menu.h"
#include "src/apps/files/TextViewer.h"
#include "src/core/StorageManager.h"

enum class FileScreen : uint8_t {
    BROWSER = 0,
    VIEWER
};

class FilesApp : public App {
public:
    FilesApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    void loadDirectory(const char* path);
    void enterDirectory(const char* name);
    void goUpDirectory();
    
    void handleBrowserInput(AppEvent e);
    void handleViewerInput(AppEvent e);

    Menu           _menu;
    FileScreen     _screen      = FileScreen::BROWSER;
    bool           _needsRedraw = true;
    
    char           _currentPath[64] = "/";
    FileInfo       _files[32]; // Max 32 items per folder to save RAM
    uint16_t       _fileCount   = 0;
    
    TextViewer     _viewer;

    static const AppInfo _info;
};
