#pragma once
// =============================================================================
// AssetManager.h — Icon and bitmap asset loading (Phase 2 stub)
//
// Phase 1: API defined; all methods return false.
// Phase 2: SD-based icon/bitmap loading implemented here.
// Critical boot assets are always drawn programmatically (no file dependency).
// =============================================================================

#include <Arduino.h>

class AssetManager {
public:
    static AssetManager& instance();

    void init();
    bool isAvailable() const { return _available; }

    // Phase 2+: load a named 16-bit icon from SD into outBuffer
    bool loadIcon  (const char* name,
                    uint16_t* outBuffer, uint16_t w, uint16_t h);

    // Phase 2+: decode a BMP from SD into outBuffer
    bool loadBitmap(const char* path,
                    uint16_t* outBuffer, uint16_t maxW, uint16_t maxH);

private:
    AssetManager() = default;
    bool _available = false;
};
