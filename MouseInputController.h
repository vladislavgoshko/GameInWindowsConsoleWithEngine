#pragma once
#include <windows.h>
#include <string>
#include <sstream>

class MouseInputController {
public:
    struct MouseState {
        COORD position{ 0,0 };
        DWORD buttonState{ 0 };
        DWORD controlKeyState{ 0 };
        DWORD eventFlags{ 0 };
        SHORT wheelDelta{ 0 };
    };

    explicit MouseInputController(HANDLE hInput)
        : hIn(hInput) {
    }

    // Обновляем состояние мыши (неблокирующее чтение)
    void Update() {
        INPUT_RECORD inputRecord; 
        DWORD eventsRead = 0;

        DWORD eventsAvailable = 0;
        GetNumberOfConsoleInputEvents(hIn, &eventsAvailable);

        while (eventsAvailable > 0) {
            if (ReadConsoleInput(hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
                if (inputRecord.EventType == MOUSE_EVENT) {
                    const auto& me = inputRecord.Event.MouseEvent;
                    state.position = me.dwMousePosition;
                    state.buttonState = me.dwButtonState;
                    state.controlKeyState = me.dwControlKeyState;
                    state.eventFlags = me.dwEventFlags;
                    state.wheelDelta = 0;

                    if (me.dwEventFlags == MOUSE_WHEELED || me.dwEventFlags == MOUSE_HWHEELED) {
                        state.wheelDelta = static_cast<SHORT>(HIWORD(me.dwButtonState));
                    }
                }
            }
            GetNumberOfConsoleInputEvents(hIn, &eventsAvailable);
        }
    }

    // Получить текущее состояние
    MouseState GetState() const {
        return state;
    }

    // Утилиты для проверки
    bool IsLeftButtonPressed() const {
        return (state.buttonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
    }

    bool IsRightButtonPressed() const {
        return (state.buttonState & RIGHTMOST_BUTTON_PRESSED) != 0;
    }

    bool IsMoved() const {
        return state.eventFlags == MOUSE_MOVED;
    }

    bool IsDoubleClick() const {
        return state.eventFlags == DOUBLE_CLICK;
    }

    bool IsWheelScrolledUp() const {
        return state.eventFlags == MOUSE_WHEELED && state.wheelDelta > 0;
    }

    bool IsWheelScrolledDown() const {
        return state.eventFlags == MOUSE_WHEELED && state.wheelDelta < 0;
    }

private:
    HANDLE hIn;
    MouseState state;
};
