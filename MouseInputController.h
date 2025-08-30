#pragma once
#include <windows.h>

class MouseInputController {
public:
    enum class ButtonEventType {
        None,
        Pressed,
        Released,
        Hold
    };

    struct MouseState {
        COORD position{ 0,0 };
        DWORD buttonState{ 0 };     // какие кнопки зажаты
        DWORD controlKeyState{ 0 }; // Shift, Ctrl, Alt
        DWORD eventFlags{ 0 };      // движение, колесо, даблклик
        SHORT wheelDelta{ 0 };

        // события кнопок
        ButtonEventType leftEvent{ ButtonEventType::None };
        ButtonEventType rightEvent{ ButtonEventType::None };
    };

    explicit MouseInputController(HANDLE hInput)
        : hIn(hInput) {
    }

    void Update() {
        INPUT_RECORD inputRecord;
        DWORD eventsRead = 0;
        DWORD eventsAvailable = 0;
        GetNumberOfConsoleInputEvents(hIn, &eventsAvailable);

        // сбрасываем события перед новым циклом
        state.leftEvent = ButtonEventType::None;
        state.rightEvent = ButtonEventType::None;
        state.wheelDelta = 0;

        while (eventsAvailable > 0) {
            if (ReadConsoleInput(hIn, &inputRecord, 1, &eventsRead) && eventsRead > 0) {
                if (inputRecord.EventType == MOUSE_EVENT) {
                    const auto& me = inputRecord.Event.MouseEvent;
                    state.position = me.dwMousePosition;
                    state.controlKeyState = me.dwControlKeyState;
                    state.eventFlags = me.dwEventFlags;

                    // колесо
                    if (me.dwEventFlags == MOUSE_WHEELED || me.dwEventFlags == MOUSE_HWHEELED) {
                        state.wheelDelta = static_cast<SHORT>(HIWORD(me.dwButtonState));
                    }

                    // кнопки (только при изменении)
                    if (me.dwEventFlags == 0) {
                        bool wasLeftDown = (state.buttonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
                        bool isLeftDown = (me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
                        if (!wasLeftDown && isLeftDown) state.leftEvent = ButtonEventType::Pressed;
                        if (wasLeftDown && !isLeftDown) state.leftEvent = ButtonEventType::Released;

                        bool wasRightDown = (state.buttonState & RIGHTMOST_BUTTON_PRESSED) != 0;
                        bool isRightDown = (me.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;
                        if (!wasRightDown && isRightDown) state.rightEvent = ButtonEventType::Pressed;
                        if (wasRightDown && !isRightDown) state.rightEvent = ButtonEventType::Released;

                        state.buttonState = me.dwButtonState;
                    }
                }
            }
            GetNumberOfConsoleInputEvents(hIn, &eventsAvailable);
        }

        // если кнопка удерживается → событие Hold
        if (IsLeftButtonDown() && state.leftEvent == ButtonEventType::None) {
            state.leftEvent = ButtonEventType::Hold;
        }
        if (IsRightButtonDown() && state.rightEvent == ButtonEventType::None) {
            state.rightEvent = ButtonEventType::Hold;
        }
    }

    MouseState GetState() const {
        return state;
    }

    bool IsLeftButtonDown() const {
        return (state.buttonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
    }

    bool IsRightButtonDown() const {
        return (state.buttonState & RIGHTMOST_BUTTON_PRESSED) != 0;
    }

    bool IsWheelScrolledUp() const {
        return state.eventFlags == MOUSE_WHEELED && state.wheelDelta > 0;
    }

    bool IsWheelScrolledDown() const {
        return state.eventFlags == MOUSE_WHEELED && state.wheelDelta < 0;
    }

    bool IsDoubleClick() const {
        return state.eventFlags == DOUBLE_CLICK;
    }

    bool IsMoved() const {
        return state.eventFlags == MOUSE_MOVED;
    }

private:
    HANDLE hIn;
    MouseState state;
};
