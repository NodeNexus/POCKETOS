#include "src/ui/Menu.h"
#include "src/ui/Widgets.h"
#include <string.h>

Menu::Menu() { clear(); }

void Menu::clear() {
    memset(_items, 0, sizeof(_items));
    _count = _selected = _scrollOffset = 0;
}

bool Menu::addItem(const char* label, const char* value, bool enabled) {
    if (_count >= MENU_MAX_ITEMS) return false;
    strncpy(_items[_count].label, label, sizeof(_items[_count].label) - 1);
    strncpy(_items[_count].value, value, sizeof(_items[_count].value) - 1);
    _items[_count].enabled = enabled;
    _count++;
    return true;
}

void Menu::updateValue(uint8_t index, const char* value) {
    if (index < _count)
        strncpy(_items[index].value, value, sizeof(_items[index].value) - 1);
}

void Menu::selectNext() {
    if (!_count) return;
    _selected = (_selected + 1) % _count;
    int16_t vis = (CONTENT_HEIGHT - 4) / MENU_ROW_H;
    ensureVisible(vis);
}

void Menu::selectPrev() {
    if (!_count) return;
    _selected = _selected ? _selected - 1 : _count - 1;
    int16_t vis = (CONTENT_HEIGHT - 4) / MENU_ROW_H;
    ensureVisible(vis);
}

void Menu::setSelected(uint8_t index) {
    if (index < _count) {
        _selected = index;
        int16_t vis = (CONTENT_HEIGHT - 4) / MENU_ROW_H;
        ensureVisible(vis);
    }
}

void Menu::ensureVisible(int16_t visibleRows) {
    if (_selected < _scrollOffset)
        _scrollOffset = _selected;
    else if (_selected >= _scrollOffset + visibleRows)
        _scrollOffset = _selected - visibleRows + 1;
}

void Menu::render(TFT_eSPI& tft, int16_t startY, int16_t height) {
    const ThemeColors& c = Theme::instance().colors();
    int16_t visibleRows  = height / MENU_ROW_H;
    ensureVisible(visibleRows);

    for (int16_t i = 0; i < visibleRows; i++) {
        uint8_t idx = _scrollOffset + (uint8_t)i;
        if (idx >= _count) break;

        const MenuItem& item  = _items[idx];
        int16_t  rowY         = startY + i * MENU_ROW_H;
        bool     sel          = (idx == _selected);

        // Row background
        if (sel) {
            tft.fillRoundRect(2, rowY, SCREEN_WIDTH - 4, MENU_ROW_H - 2,
                              3, c.selection);
            // Left accent bar
            tft.fillRect(2, rowY, 3, MENU_ROW_H - 2, c.selectionBorder);
        }

        // Label
        uint16_t labelColor = item.enabled
            ? (sel ? c.text : c.text)
            : c.textDisabled;
        tft.setTextColor(labelColor);
        tft.drawString(item.label, 10, rowY + (MENU_ROW_H - 8) / 2, 1);

        // Right-aligned value
        if (item.value[0]) {
            tft.setTextColor(sel ? c.primary : c.textMuted);
            int16_t vw = tft.textWidth(item.value, 1);
            tft.drawString(item.value, SCREEN_WIDTH - vw - 6,
                           rowY + (MENU_ROW_H - 8) / 2, 1);
        }
    }

    // Scrollbar
    if (_count > (uint8_t)visibleRows) {
        Widgets::drawScrollbar(tft, SCREEN_WIDTH - 3, startY, height,
                               _count, (uint8_t)visibleRows, _scrollOffset,
                               c.border, c.primary);
    }
}
