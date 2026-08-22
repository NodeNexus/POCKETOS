#pragma once
// =============================================================================
// Dialog.h — Modal confirmation dialog overlay
//
// Renders a centred dialog box with title, message, and two buttons.
// Default focus is on the Cancel/No button for safety.
// Caller checks getResult() each frame and calls dismiss() when handled.
// =============================================================================

#include <TFT_eSPI.h>
#include <Arduino.h>
#include "src/apps/App.h"   // AppEvent
#include "src/ui/Theme.h"
#include "src/board_config.h"

enum class DialogResult : uint8_t {
    PENDING   = 0,
    CONFIRMED,
    CANCELLED
};

class Dialog {
public:
    Dialog() = default;

    // Show the dialog; focusConfirm=false defaults to safer "No" option
    void show(const char* title,
              const char* message,
              const char* confirmLabel = "Yes",
              const char* cancelLabel  = "No",
              bool        focusConfirm = false);

    void         handleInput(AppEvent event);
    void         render(TFT_eSPI& tft);

    bool         isActive()   const { return _active; }
    DialogResult getResult()  const { return _result; }
    void         dismiss()          { _active = false; _result = DialogResult::PENDING; }

private:
    bool         _active       = false;
    DialogResult _result       = DialogResult::PENDING;
    bool         _focusYes     = false;  // false = focused on No (safe default)

    char _title       [32];
    char _message     [80];
    char _confirmLabel[14];
    char _cancelLabel [14];
};
