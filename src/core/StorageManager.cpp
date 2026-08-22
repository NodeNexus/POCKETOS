#include "src/core/StorageManager.h"
#include "src/debug.h"
#include "src/system_config.h"

StorageManager& StorageManager::instance() {
    static StorageManager inst;
    return inst;
}

bool StorageManager::init() {
    LOG_INFO("StorageManager: mounting SD (CS=%d)", PIN_SD_CS);

    // SD.begin() uses the default SPI instance shared with TFT.
    // Separate CS pins ensure the TFT and SD are never selected simultaneously.
    if (!SD.begin(PIN_SD_CS)) {
        _status = SDStatus::NOT_PRESENT;
        LOG_WARN("StorageManager: SD not found or mount failed");
        return false;
    }

    if (SD.cardType() == CARD_NONE) {
        _status = SDStatus::NOT_PRESENT;
        LOG_WARN("StorageManager: no card detected");
        return false;
    }

    _status = SDStatus::OK;
    LOG_INFO("StorageManager: SD OK  type=%d  size=%lu MB",
             SD.cardType(), (unsigned long)cardSizeMB());

    ensureDirectories();
    return true;
}

bool StorageManager::remount() {
    SD.end();
    return init();
}

bool StorageManager::readFile(const char* path, String& out) {
    if (!isAvailable()) { LOG_WARN("SD read skipped — not available"); return false; }
    File f = SD.open(path, FILE_READ);
    if (!f) { LOG_WARN("StorageManager: cannot open '%s'", path); return false; }
    out = f.readString();
    f.close();
    return true;
}

bool StorageManager::writeFile(const char* path, const char* content, bool append) {
    if (!isAvailable()) { LOG_WARN("SD write skipped — not available"); return false; }
    const char* mode = append ? FILE_APPEND : FILE_WRITE;
    File f = SD.open(path, mode);
    if (!f) { LOG_WARN("StorageManager: cannot open '%s' for write", path); return false; }
    f.print(content);
    f.close();
    return true;
}

bool StorageManager::fileExists(const char* path) {
    return isAvailable() && SD.exists(path);
}

bool StorageManager::deleteFile(const char* path) {
    return isAvailable() && SD.remove(path);
}

bool StorageManager::createDir(const char* path) {
    if (!isAvailable()) return false;
    if (SD.exists(path)) return true;
    return SD.mkdir(path);
}

File StorageManager::openFile(const char* path, const char* mode) {
    if (!isAvailable()) return File();
    return SD.open(path, mode);
}

bool StorageManager::listDir(const char* path, FileInfo* outArray, uint16_t maxItems, uint16_t& outCount) {
    outCount = 0;
    if (!isAvailable() || !outArray || maxItems == 0) return false;

    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) {
        if (dir) dir.close();
        return false;
    }

    File file = dir.openNextFile();
    while (file && outCount < maxItems) {
        // Strip the path to get just the filename
        const char* fullName = file.name();
        const char* baseName = strrchr(fullName, '/');
        if (baseName) baseName++; // Skip the slash
        else baseName = fullName;

        strncpy(outArray[outCount].name, baseName, sizeof(outArray[outCount].name) - 1);
        outArray[outCount].name[sizeof(outArray[outCount].name) - 1] = '\0';
        outArray[outCount].isDirectory = file.isDirectory();
        outArray[outCount].size = file.size();

        outCount++;
        file.close();
        file = dir.openNextFile();
    }
    
    if (file) file.close();
    dir.close();
    return true;
}

uint64_t StorageManager::totalBytes()  const { return isAvailable() ? SD.totalBytes() : 0; }
uint64_t StorageManager::usedBytes()   const { return isAvailable() ? SD.usedBytes()  : 0; }
uint64_t StorageManager::freeBytes()   const {
    if (!isAvailable()) return 0;
    uint64_t t = SD.totalBytes(), u = SD.usedBytes();
    return (u < t) ? (t - u) : 0;
}
uint32_t StorageManager::cardSizeMB()  const {
    return isAvailable() ? (uint32_t)(SD.cardSize() / (1024ULL * 1024ULL)) : 0;
}

void StorageManager::ensureDirectories() {
    static const char* dirs[] = {
        SD_POCKETOS_DIR, SD_CONFIG_DIR, SD_SAVES_DIR,
        SD_PHOTOS_DIR,   SD_MUSIC_DIR,  SD_LOGS_DIR,
        SD_CACHE_DIR,    nullptr
    };
    for (int i = 0; dirs[i]; i++) {
        if (!SD.exists(dirs[i])) {
            if (SD.mkdir(dirs[i])) {
                LOG_DEBUG("SD: created %s", dirs[i]);
            } else {
                LOG_WARN("SD: failed to create %s", dirs[i]);
            }
        }
    }
}
