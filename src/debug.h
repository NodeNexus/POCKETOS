#pragma once
// =============================================================================
// debug.h — PocketOS Logging Macros
//
// Usage:
//   LOG_INFO("SD mounted, %d files found", count);
//   LOG_WARN("Network timeout after %d ms", elapsed);
//   LOG_ERROR("Display init failed");
//   LOG_DEBUG("Button state: %d", state);
//
// NEVER log passwords, API keys, or credentials. Use LOG_SENSITIVE() instead.
// =============================================================================

#include <Arduino.h>

// Log level constants
#define LOG_LEVEL_NONE    0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4

// Determine active log level from build flags
#ifdef POCKETOS_DEBUG
  #define POCKETOS_LOG_LEVEL LOG_LEVEL_DEBUG
#else
  #define POCKETOS_LOG_LEVEL LOG_LEVEL_ERROR
#endif

// ANSI colour codes (disable if your terminal does not support them)
#define _LC_RESET   "\033[0m"
#define _LC_RED     "\033[31m"
#define _LC_YELLOW  "\033[33m"
#define _LC_CYAN    "\033[36m"
#define _LC_GRAY    "\033[37m"

// ---------------------------------------------------------------------------
// Logging macros
// ---------------------------------------------------------------------------
#if POCKETOS_LOG_LEVEL >= LOG_LEVEL_ERROR
  #define LOG_ERROR(fmt, ...) \
    Serial.printf(_LC_RED "[ERR][%s:%d] " fmt _LC_RESET "\n", \
                  __FILE__, __LINE__, ##__VA_ARGS__)
#else
  #define LOG_ERROR(fmt, ...) do {} while(0)
#endif

#if POCKETOS_LOG_LEVEL >= LOG_LEVEL_WARN
  #define LOG_WARN(fmt, ...) \
    Serial.printf(_LC_YELLOW "[WRN][%s:%d] " fmt _LC_RESET "\n", \
                  __FILE__, __LINE__, ##__VA_ARGS__)
#else
  #define LOG_WARN(fmt, ...) do {} while(0)
#endif

#if POCKETOS_LOG_LEVEL >= LOG_LEVEL_INFO
  #define LOG_INFO(fmt, ...) \
    Serial.printf(_LC_CYAN "[INF] " fmt _LC_RESET "\n", ##__VA_ARGS__)
#else
  #define LOG_INFO(fmt, ...) do {} while(0)
#endif

#if POCKETOS_LOG_LEVEL >= LOG_LEVEL_DEBUG
  #define LOG_DEBUG(fmt, ...) \
    Serial.printf(_LC_GRAY "[DBG][%s:%d] " fmt _LC_RESET "\n", \
                  __FILE__, __LINE__, ##__VA_ARGS__)
#else
  #define LOG_DEBUG(fmt, ...) do {} while(0)
#endif

// Redact sensitive fields — prints field name but never the value
#define LOG_SENSITIVE(field_name) \
  LOG_DEBUG("(sensitive: %s) [REDACTED]", field_name)
