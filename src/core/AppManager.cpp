#include "src/core/AppManager.h"
#include "src/core/InputManager.h"
#include "src/debug.h"
#include <string.h>

AppManager& AppManager::instance() {
    static AppManager inst;
    return inst;
}

void AppManager::init(App* launcher) {
    memset(_stack, 0, sizeof(_stack));
    _stackDepth   = 1;
    _stack[0]     = launcher;
    launcher->onCreate();
    launcher->onEnter();
    LOG_INFO("AppManager: init with launcher '%s'", launcher->getInfo().name);
}

void AppManager::update(uint32_t deltaMs) {
    App* app = currentApp();
    if (!app) return;

    InputManager& input = InputManager::instance();

    // Drain the event queue
    while (input.hasEvent()) {
        AppEvent evt = input.popEvent();

        // System-level back/home — only when not already at launcher
        if (_stackDepth > 1) {
            if (evt == AppEvent::BTN_SELECT_LONG) {
                exitCurrentApp();
                return;
            }
            if (evt == AppEvent::BTN_SELECT_VERY_LONG) {
                goHome();
                return;
            }
        }

        app->handleInput(evt);
    }

    app->update(deltaMs);
    processFlags(app);
}

void AppManager::processFlags(App* app) {
    if (app->requestHome) {
        app->requestHome = false;
        goHome();
        return;
    }
    if (app->requestExit) {
        app->requestExit = false;
        exitCurrentApp();
    }
}

void AppManager::launchApp(App* app) {
    if (!app) return;
    if (_stackDepth >= APP_STACK_MAX_DEPTH) {
        LOG_WARN("AppManager: stack full — cannot launch '%s'", app->getInfo().name);
        return;
    }
    App* prev = currentApp();
    if (prev) prev->onExit();

    app->onCreate();
    _stack[_stackDepth++] = app;
    app->onEnter();

    InputManager::instance().clearEvents();
    LOG_INFO("AppManager: launched '%s' (depth=%d)", app->getInfo().name, _stackDepth);
}

void AppManager::exitCurrentApp() {
    if (_stackDepth <= 1) return;

    App* dying = currentApp();
    dying->onExit();
    _stackDepth--;

    App* prev = currentApp();
    if (prev) prev->onEnter();

    InputManager::instance().clearEvents();
    LOG_INFO("AppManager: exited to '%s' (depth=%d)",
             prev ? prev->getInfo().name : "none", _stackDepth);
}

void AppManager::goHome() {
    while (_stackDepth > 1) {
        currentApp()->onExit();
        _stackDepth--;
    }
    if (currentApp()) currentApp()->onEnter();
    InputManager::instance().clearEvents();
    LOG_INFO("AppManager: → home (depth=%d)", _stackDepth);
}

App* AppManager::currentApp() const {
    return (_stackDepth > 0) ? _stack[_stackDepth - 1] : nullptr;
}
