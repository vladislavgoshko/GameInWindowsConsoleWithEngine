#pragma once

#include <windows.h>

class MouseInputController {
public:
    enum class ButtonEventType {
        None,
        Pressed,
        Released,
        Hold,
        DoubleClick
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

    explicit MouseInputController(HANDLE hInput);

    void Update();
    MouseState GetState() const;

    bool IsLeftButtonDown() const;
    bool IsRightButtonDown() const;
    bool IsWheelScrolledUp() const;
    bool IsWheelScrolledDown() const;
    bool IsDoubleClick() const;
    bool IsMoved() const;

private:
    HANDLE hIn;
    MouseState state;
};
