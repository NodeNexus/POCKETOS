#include "src/ui/Launcher.h"
#include "src/ui/StatusBar.h"
#include "src/core/AppManager.h"
#include "src/debug.h"
#include <string.h>

const AppInfo Launcher::_info = { 0, "Launcher", nullptr };

Launcher::Launcher() {
    memset(_apps, 0, sizeof(_apps));
}

bool Launcher::registerApp(App* app) {
    if (!app || _appCount >= LAUNCHER_MAX_APPS) {
        LOG_WARN("Launcher: cannot register app (full or null)");
        return false;
    }
    _apps[_appCount++] = app;
    LOG_DEBUG("Launcher: registered '%s' [%d]", app->getInfo().name, _appCount - 1);
    return true;
}

App* Launcher::getApp(uint8_t index) const {
    return (index < _appCount) ? _apps[index] : nullptr;
}

void Launcher::onCreate() { LOG_INFO("Launcher: onCreate"); }

void Launcher::onEnter() {
    _needsRedraw = true;
    StatusBar::instance().markDirty();
    LOG_DEBUG("Launcher: onEnter (%d apps)", _appCount);
}

void Launcher::handleInput(AppEvent event) {
    uint8_t prev   = _selected;
    uint8_t perPage = LAUNCHER_COLS * LAUNCHER_ROWS;

    switch (event) {
        case AppEvent::BTN_UP_SHORT:
        case AppEvent::BTN_UP_HELD:
            _selected = (_selected > 0) ? _selected - 1 : (_appCount > 0 ? _appCount - 1 : 0);
            break;

        case AppEvent::BTN_DOWN_SHORT:
        case AppEvent::BTN_DOWN_HELD:
            if (_appCount > 0) _selected = (_selected + 1) % _appCount;
            break;

        case AppEvent::BTN_SELECT_SHORT:
            if (_selected < _appCount && _apps[_selected]) {
                LOG_INFO("Launcher: launching '%s'", _apps[_selected]->getInfo().name);
                AppManager::instance().launchApp(_apps[_selected]);
                return;
            }
            break;

        default:
            return;
    }

    if (_selected != prev) {
        // Update page
        while (_selected < _pageStart)
            _pageStart = (_pageStart >= LAUNCHER_COLS) ? _pageStart - LAUNCHER_COLS : 0;
        while (_selected >= _pageStart + perPage)
            _pageStart += LAUNCHER_COLS;

        _needsRedraw = true;
    }
}

void Launcher::update(uint32_t /*deltaMs*/) {
    StatusBar::instance().update();
    if (StatusBar::instance().isDirty()) _needsRedraw = true;
}

void Launcher::render(TFT_eSPI& tft) {
    StatusBar::instance().render(tft);
    if (!_needsRedraw) return;
    _needsRedraw = false;

    const ThemeColors& c = Theme::instance().colors();

    // Clear content area
    tft.fillRect(0, STATUS_BAR_HEIGHT, SCREEN_WIDTH, CONTENT_HEIGHT, c.background);

    drawGrid(tft);

    // Page indicator dots
    uint8_t perPage    = LAUNCHER_COLS * LAUNCHER_ROWS;
    uint8_t totalPages = (_appCount + perPage - 1) / perPage;
    if (totalPages > 1) {
        uint8_t curPage = _pageStart / perPage;
        int16_t dotY    = SCREEN_HEIGHT - 5;
        int16_t startX  = (SCREEN_WIDTH - totalPages * 6) / 2;
        for (uint8_t p = 0; p < totalPages; p++) {
            tft.fillCircle(startX + p * 6, dotY, 2,
                           (p == curPage) ? c.primary : c.border);
        }
    }
}

void Launcher::drawGrid(TFT_eSPI& tft) {
    uint8_t perPage = LAUNCHER_COLS * LAUNCHER_ROWS;
    uint8_t end     = min((uint8_t)(_pageStart + perPage), _appCount);

    for (uint8_t i = _pageStart; i < end; i++) {
        uint8_t local = i - _pageStart;
        uint8_t col   = local % LAUNCHER_COLS;
        uint8_t row   = local / LAUNCHER_COLS;
        drawCell(tft, col, row, i, i == _selected);
    }
}

void Launcher::drawCell(TFT_eSPI& tft, uint8_t col, uint8_t row,
                        uint8_t appIdx, bool selected) {
    const ThemeColors& c = Theme::instance().colors();
    int16_t cx = cellCX(col);
    int16_t cy = cellCY(row);

    if (selected) {
        // Highlight background for selected cell
        int16_t hx = col * CELL_W + 2;
        int16_t hy = GRID_TOP + row * CELL_H + 1;
        tft.fillRoundRect(hx, hy, CELL_W - 4, CELL_H - 2, 5, c.surfaceVariant);
        tft.drawRoundRect(hx, hy, CELL_W - 4, CELL_H - 2, 5, c.selectionBorder);
    }

    App* app = _apps[appIdx];
    if (!app) return;

    // Draw icon
    uint16_t iconColor = selected ? c.primary : c.text;
    if (app->getInfo().drawIcon) {
        app->getInfo().drawIcon(tft, cx, cy, iconColor);
    } else {
        // Fallback: circle with initial letter
        tft.drawCircle(cx, cy, 8, iconColor);
        char init[2] = { app->getInfo().name[0], '\0' };
        tft.setTextColor(iconColor);
        tft.drawString(init, cx - 3, cy - 6, 1);
    }

    // App name below icon
    const char* name = app->getInfo().name;
    tft.setTextColor(selected ? c.primary : c.textMuted);
    int16_t nw  = tft.textWidth(name, 1);
    int16_t nx  = cx - nw / 2;
    // Clamp to cell bounds
    if (nx < col * CELL_W + 1) nx = col * CELL_W + 1;
    tft.drawString(name, nx, cy + 12, 1);
}
