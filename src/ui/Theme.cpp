#include "src/ui/Theme.h"

Theme& Theme::instance() {
    static Theme inst;
    return inst;
}

void Theme::init(ThemeId id) {
    setTheme(id);
}

void Theme::setTheme(ThemeId id) {
    _currentId = id;
    switch (id) {
        case ThemeId::DARK:  _colors = Themes::DARK;  break;
        case ThemeId::LIGHT: _colors = Themes::LIGHT; break;
        case ThemeId::CYBER: _colors = Themes::CYBER; break;
        default:             _colors = Themes::DARK;  break;
    }
}
