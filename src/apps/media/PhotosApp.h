#pragma once
// =============================================================================
// PhotosApp.h — Photo Viewer and Slideshow
// =============================================================================

#include "src/apps/App.h"
#include "src/ui/Menu.h"
#include "src/core/StorageManager.h"

enum class PhotoScreen {
    BROWSER,
    VIEWER,
    SLIDESHOW
};

class PhotosApp : public App {
public:
    PhotosApp();

    const AppInfo& getInfo()             const override { return _info; }
    void           onCreate()                  override;
    void           onEnter()                   override;
    void           update(uint32_t deltaMs)    override;
    void           render(TFT_eSPI& tft)       override;
    void           handleInput(AppEvent event) override;
    void           onExit()                    override;

    static void drawIcon(TFT_eSPI& tft, int16_t cx, int16_t cy, uint16_t color);

private:
    void loadDirectory();
    void showImage(int index);

    Menu           _menu;
    PhotoScreen    _screen      = PhotoScreen::BROWSER;
    bool           _needsRedraw = true;
    
    FileInfo       _files[32];
    uint16_t       _fileCount   = 0;
    
    int            _currentImageIndex = -1;
    uint32_t       _slideshowTimerMs = 0;
    
    static const AppInfo _info;
};
