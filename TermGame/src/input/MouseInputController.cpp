#include "input\MouseInputController.h"

MouseInputController::MouseInputController(HANDLE hInput)
    : hIn(hInput) {
}

void MouseInputController::Update() {
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

                // обработка даблклика
                if (me.dwEventFlags == DOUBLE_CLICK) {
                    if (me.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) {
                        state.leftEvent = ButtonEventType::DoubleClick;
                        // добавить ещё обычный Pressed
                        state.leftEvent = ButtonEventType::Pressed;
                    }
                    if (me.dwButtonState & RIGHTMOST_BUTTON_PRESSED) {
                        state.rightEvent = ButtonEventType::DoubleClick;
                        state.rightEvent = ButtonEventType::Pressed;
                    }
                    state.buttonState = me.dwButtonState;
                }
                // обычные клики
                else if (me.dwEventFlags == 0) {
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

MouseInputController::MouseState MouseInputController::GetState() const {
    return state;
}

bool MouseInputController::IsLeftButtonDown() const {
    return (state.buttonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
}

bool MouseInputController::IsRightButtonDown() const {
    return (state.buttonState & RIGHTMOST_BUTTON_PRESSED) != 0;
}

bool MouseInputController::IsWheelScrolledUp() const {
    return state.eventFlags == MOUSE_WHEELED && state.wheelDelta > 0;
}

bool MouseInputController::IsWheelScrolledDown() const {
    return state.eventFlags == MOUSE_WHEELED && state.wheelDelta < 0;
}

bool MouseInputController::IsDoubleClick() const {
    return state.eventFlags == DOUBLE_CLICK;
}

bool MouseInputController::IsMoved() const {
    return state.eventFlags == MOUSE_MOVED;
}
