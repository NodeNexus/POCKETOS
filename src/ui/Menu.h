#pragma once
// =============================================================================
// Menu.h — Vertical scrolling list selector widget
//
// Renders a scrollable list of labelled items with optional right-aligned
// value text. Highlighted row uses the theme selection colour.
// =============================================================================

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "src/ui/Theme.h"
#include "src/board_config.h"

static constexpr uint8_t  MENU_MAX_ITEMS = 24;
static constexpr int16_t  MENU_ROW_H     = 18;   // px per row

struct MenuItem {
    char  label[32];
    char  value[16];
    bool  enabled;
};

class Menu {
public:
    Menu();

    void    clear();
    bool    addItem(const char* label,
                    const char* value   = "",
                    bool        enabled = true);

    // Navigation
    void    selectNext();
    void    selectPrev();
    void    setSelected(uint8_t index);
    uint8_t getSelected() const { return _selected; }
    uint8_t getCount()    const { return _count; }

    // Render into the rectangle [x=0, y=startY, w=SCREEN_WIDTH, h=height]
    void render(TFT_eSPI& tft, int16_t startY, int16_t height);

    // Update a value string for an existing item (triggers redraw by caller)
    void updateValue(uint8_t index, const char* value);

private:
    void ensureVisible(int16_t visibleRows);

    MenuItem _items[MENU_MAX_ITEMS];
    uint8_t  _count        = 0;
    uint8_t  _selected     = 0;
    uint8_t  _scrollOffset = 0;
};
