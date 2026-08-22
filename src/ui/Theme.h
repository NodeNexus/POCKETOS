#ifndef POCKETOS_THEME_H
#define POCKETOS_THEME_H
// =============================================================================
// Theme.h — Color palette definitions and Theme manager
//
// All colors are stored as RGB565 (uint16_t) for direct TFT rendering.
// The RGB565() helper converts 8-bit RGB to RGB565 at compile time.
// =============================================================================

#include <Arduino.h>

// Compile-time RGB→RGB565 conversion (identical to TFT_eSPI's color565())
constexpr uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b) {
    return  ((uint16_t)(r >> 3) << 11)
          | ((uint16_t)(g >> 2) <<  5)
          | ((uint16_t)(b >> 3));
}

// ---------------------------------------------------------------------------
// Theme identifiers
// ---------------------------------------------------------------------------
enum class ThemeId : uint8_t {
    DARK  = 0,
    LIGHT = 1,
    CYBER = 2,
    THEME_COUNT  // Keep last — used for bounds checking
};

// ---------------------------------------------------------------------------
// ThemeColors — full palette for one theme
// ---------------------------------------------------------------------------
struct ThemeColors {
    uint16_t background;        // Main screen background
    uint16_t surface;           // Card / panel / header fill
    uint16_t surfaceVariant;    // Slightly lighter surface (hovered/selected bg)
    uint16_t primary;           // Primary accent (cyan / blue / magenta)
    uint16_t primaryDim;        // Dimmed version of primary
    uint16_t text;              // Primary text
    uint16_t textMuted;         // Secondary / hint text
    uint16_t textDisabled;      // Greyed-out labels
    uint16_t success;           // Positive indicator (green)
    uint16_t warning;           // Warning indicator (amber)
    uint16_t error;             // Error / danger (red)
    uint16_t selection;         // Selection highlight fill
    uint16_t selectionBorder;   // Selection highlight border
    uint16_t border;            // Dividers and subtle outlines
    uint16_t statusBar;         // Status bar background
    uint16_t statusBarText;     // Status bar text / icons
};

// ---------------------------------------------------------------------------
// Built-in palette constants (computed at compile time)
// ---------------------------------------------------------------------------
namespace Themes {

    constexpr ThemeColors DARK = {
        /* background    */ RGB565( 13,  17,  23),   // #0D1117 deep navy-black
        /* surface       */ RGB565( 22,  27,  34),   // #161B22 dark navy
        /* surfaceVariant*/ RGB565( 30,  42,  58),   // #1E2A3A mid navy
        /* primary       */ RGB565(  0, 180, 216),   // #00B4D8 cyan
        /* primaryDim    */ RGB565(  0,  90, 120),   // dimmed cyan
        /* text          */ RGB565(220, 224, 232),   // #DCE0E8 near-white
        /* textMuted     */ RGB565(100, 112, 130),   // #64707E medium-gray
        /* textDisabled  */ RGB565( 50,  55,  65),   // dim gray
        /* success       */ RGB565( 60, 200, 100),   // #3CC864 green
        /* warning       */ RGB565(255, 165,   0),   // #FFA500 amber
        /* error         */ RGB565(240,  70,  80),   // #F04650 red
        /* selection     */ RGB565(  0,  72, 128),   // #004880 blue selection
        /* selectionBorder*/ RGB565(  0, 180, 216),  // #00B4D8 cyan border
        /* border        */ RGB565( 38,  46,  60),   // #262E3C subtle outline
        /* statusBar     */ RGB565(  8,  10,  16),   // darker than background
        /* statusBarText */ RGB565(140, 155, 175),   // #8C9BAF muted blue-gray
    };

    constexpr ThemeColors LIGHT = {
        /* background    */ RGB565(238, 242, 248),   // #EEF2F8 off-white
        /* surface       */ RGB565(255, 255, 255),   // white
        /* surfaceVariant*/ RGB565(215, 228, 242),   // light blue-gray
        /* primary       */ RGB565(  0,  96, 200),   // #0060C8 blue
        /* primaryDim    */ RGB565( 90, 150, 220),   // light blue
        /* text          */ RGB565( 20,  24,  32),   // near-black
        /* textMuted     */ RGB565( 90, 100, 120),   // medium gray
        /* textDisabled  */ RGB565(175, 180, 192),   // light gray
        /* success       */ RGB565( 30, 150,  70),   // green
        /* warning       */ RGB565(190, 110,   0),   // amber
        /* error         */ RGB565(200,  40,  50),   // red
        /* selection     */ RGB565(175, 212, 255),   // light blue selection
        /* selectionBorder*/ RGB565(  0,  96, 200),  // blue border
        /* border        */ RGB565(195, 205, 220),   // light outline
        /* statusBar     */ RGB565( 25,  55,  95),   // dark blue status bar
        /* statusBarText */ RGB565(240, 242, 248),   // near-white
    };

    constexpr ThemeColors CYBER = {
        /* background    */ RGB565(  8,   2,  15),   // #08020F near-black purple
        /* surface       */ RGB565( 18,   5,  30),   // #12051E dark purple
        /* surfaceVariant*/ RGB565( 35,  12,  55),   // medium purple
        /* primary       */ RGB565(255,   0, 240),   // #FF00F0 hot magenta
        /* primaryDim    */ RGB565(130,   0, 120),   // dim magenta
        /* text          */ RGB565(240, 240, 255),   // near-white
        /* textMuted     */ RGB565(155,  95, 195),   // purple muted
        /* textDisabled  */ RGB565( 75,  45,  95),   // dark purple
        /* success       */ RGB565(  0, 255, 150),   // #00FF96 neon green
        /* warning       */ RGB565(255, 200,   0),   // neon yellow
        /* error         */ RGB565(255,  50,  50),   // bright red
        /* selection     */ RGB565( 75,   0,  95),   // purple selection
        /* selectionBorder*/ RGB565(255,   0, 240),  // magenta border
        /* border        */ RGB565( 58,  18,  78),   // dark purple outline
        /* statusBar     */ RGB565(  4,   0,   8),   // near-black
        /* statusBarText */ RGB565(255,   0, 240),   // magenta text
    };

} // namespace Themes

// ---------------------------------------------------------------------------
// Theme singleton — manages the active palette
// ---------------------------------------------------------------------------
class Theme {
public:
    static Theme& instance();

    void    init(ThemeId id = ThemeId::DARK);
    void    setTheme(ThemeId id);

    ThemeId          currentId()  const { return _currentId; }
    const ThemeColors& colors()   const { return _colors; }

    // Convenience shortcuts
    uint16_t bg()            const { return _colors.background; }
    uint16_t surface()       const { return _colors.surface; }
    uint16_t primary()       const { return _colors.primary; }
    uint16_t text()          const { return _colors.text; }
    uint16_t textMuted()     const { return _colors.textMuted; }
    uint16_t selection()     const { return _colors.selection; }
    uint16_t success()       const { return _colors.success; }
    uint16_t warning()       const { return _colors.warning; }
    uint16_t error()         const { return _colors.error; }

private:
    Theme() = default;
    ThemeColors _colors    = Themes::DARK;
    ThemeId     _currentId = ThemeId::DARK;
};

#endif // POCKETOS_THEME_H
