#ifndef POCKETOS_STORAGE_MANAGER_H
#define POCKETOS_STORAGE_MANAGER_H
// =============================================================================
// StorageManager.h — SD card access and safe file I/O
//
// SD card shares the SPI bus with the TFT display. Each device has its own
// CS pin. SD access happens between render frames (cooperative scheduling),
// so concurrent SPI conflicts are avoided by design in Phase 1.
// A SPI mutex should be added if FreeRTOS tasks are introduced later.
//
// All public methods degrade gracefully when the SD card is absent.
// =============================================================================

#include <Arduino.h>
#include <SD.h>
#include <FS.h>
#include "src/board_config.h"

enum class SDStatus : uint8_t {
    NOT_INITIALIZED,
    NOT_PRESENT,
    OK,
    ERROR
};

struct FileInfo {
    char     name[64];
    uint32_t size;
    bool     isDirectory;
};

class StorageManager {
public:
    static StorageManager& instance();

    // Attempt to mount SD card; returns false if absent (non-fatal)
    bool init();

    // Re-attempt mount (call if SD status is NOT_PRESENT)
    bool remount();

    SDStatus getStatus()   const { return _status; }
    bool     isAvailable() const { return _status == SDStatus::OK; }

    // -----------------------------------------------------------------------
    // File operations — all return false if SD unavailable
    // -----------------------------------------------------------------------
    bool readFile(const char* path, String& out);
    bool writeFile(const char* path, const char* content, bool append = false);
    bool fileExists(const char* path);
    bool deleteFile(const char* path);
    bool createDir(const char* path);

    // Stream access (caller must close)
    File openFile(const char* path, const char* mode = FILE_READ);

    // List directory up to maxItems (populates outArray, sets outCount)
    bool listDir(const char* path, FileInfo* outArray, uint16_t maxItems, uint16_t& outCount);

    // -----------------------------------------------------------------------
    // Card information
    // -----------------------------------------------------------------------
    uint64_t totalBytes()  const;
    uint64_t usedBytes()   const;
    uint64_t freeBytes()   const;
    uint32_t cardSizeMB()  const;

    // Create the PocketOS directory tree on first mount
    void ensureDirectories();

private:
    StorageManager() = default;

    SDStatus _status = SDStatus::NOT_INITIALIZED;
};

#endif // POCKETOS_STORAGE_MANAGER_H
