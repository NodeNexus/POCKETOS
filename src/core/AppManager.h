#pragma once
// =============================================================================
// AppManager.h — Application lifecycle and stack navigation
//
// Maintains a stack of active apps (max APP_STACK_MAX_DEPTH deep).
// The Launcher always lives at position 0.
//
// Default system navigation (handled here before forwarding to app):
//   BTN_SELECT_LONG      → exitCurrentApp()  (when stack depth > 1)
//   BTN_SELECT_VERY_LONG → goHome()          (when stack depth > 1)
//
// Apps can consume SELECT_LONG in their own handleInput() to prevent this.
// =============================================================================

#include <Arduino.h>
#include "src/apps/App.h"
#include "src/system_config.h"

class AppManager {
public:
    static AppManager& instance();

    // Register the launcher as the permanent stack base
    void init(App* launcher);

    // Main cooperative update — dispatches input, runs update, checks flags
    void update(uint32_t deltaMs);

    // Push a new app onto the stack
    void launchApp(App* app);

    // Pop the current app; return to previous
    void exitCurrentApp();

    // Clear the stack above the launcher; return home
    void goHome();

    // Accessors
    App*    currentApp()        const;
    bool    isLauncherActive()  const { return _stackDepth == 1; }
    uint8_t stackDepth()        const { return _stackDepth; }

private:
    AppManager() = default;
    void processFlags(App* app);

    App*    _stack[APP_STACK_MAX_DEPTH];
    uint8_t _stackDepth = 0;
};
