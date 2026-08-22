#include "src/core/AssetManager.h"
#include "src/core/StorageManager.h"
#include "src/debug.h"

AssetManager& AssetManager::instance() {
    static AssetManager inst;
    return inst;
}

void AssetManager::init() {
    _available = StorageManager::instance().isAvailable();
    LOG_INFO("AssetManager: init  SD available=%d", _available);
}

bool AssetManager::loadIcon(const char* name,
                             uint16_t* outBuffer, uint16_t w, uint16_t h) {
    // Phase 2 TODO: read icon file from SD_POCKETOS_DIR/icons/<name>.bin
    (void)name; (void)outBuffer; (void)w; (void)h;
    return false;
}

bool AssetManager::loadBitmap(const char* path,
                               uint16_t* outBuffer, uint16_t maxW, uint16_t maxH) {
    // Phase 2 TODO: decode 16-bit BMP from SD
    (void)path; (void)outBuffer; (void)maxW; (void)maxH;
    return false;
}
