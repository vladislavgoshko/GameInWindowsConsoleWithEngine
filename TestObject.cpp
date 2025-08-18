#include "TestObject.h"


TestObject::TestObject(COORD startPos, float playerSpeed, KeyboardInputController* inputCtrl, ConsoleEngine* ce)
    : position(startPos), speed(playerSpeed), inputController(inputCtrl), ce(ce) {

    //texture = {
    //    {0, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 0, 0},
    //    {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
    //    {3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 1},
    //    {3, 3, 3, 0, 14, 3, 3, 3, 8, 0, 3, 3, 1},
    //    {3, 3, 0, 0, 0, 3, 3, 3, 0, 0, 0, 3, 1},
    //    {3, 3, 0, 3, 0, 3, 3, 3, 0, 3, 0, 3, 1},
    //    {3, 3, 3, 3, 3, 0, 3, 0, 3, 3, 3, 3, 1},
    //    {0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0},
    //    {0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 0},
    //    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    //    {0, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 0},
    //    {0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0},
    //    {0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0}
    //};

    texture = {
    {0,0,0,0,6,6,6,6,6,0,0,0,0},      // волосы
    {0,0,0,6,14,14,14,14,6,0,0,0,0},  // голова
    {0,0,6,14,14,14,14,14,14,6,0,0,0},
    {0,0,6,14,14,14,14,14,14,6,0,0,0},
    {0,0,0,0,0,14,14,0,0,0,0,0,0},    // шея
    {0,0,1,1,1,1,1,1,1,0,0,0,0},      // торс
    {0,14,1,1,1,1,1,1,1,1,14,0,0},    // руки
    {0,14,1,1,1,4,4,1,1,1,14,0,0},    // ремень
    {0,0,1,1,1,1,1,1,1,0,0,0,0},
    {0,0,8,8,8,8,8,8,8,0,0,0,0},      // штаны
    {0,0,8,8,8,0,0,8,8,0,0,0,0},      // ноги
    {0,0,8,8,8,0,0,8,8,0,0,0,0},
    {0,0,3,3,0,0,0,0,3,3,0,0,0}       // обувь
    };

}

const std::vector<std::vector<SHORT>>& TestObject::GetTexture() const {
    return texture;
}

COORD TestObject::GetPosition() const {
    return position;
}

void TestObject::SetPosition(COORD newPosition) {
    position = newPosition;
}

void TestObject::Update(float deltaTime) {
    if (inputController->IsKeyPressed(VK_SPACE)) {
        ce->SwitchFullscreen(!ce->IsFullcreen);
    }
}

void TestObject::FixedUpdate(float deltaTime) {
    auto currentTP = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsedMoveTime = currentTP - lastMoveTP;

    if (elapsedMoveTime.count() > 1.0f / speed) {
        if (inputController->IsKeyHeld(VK_UP) || inputController->IsKeyPressed(VK_UP)) {
            position.Y--;
            lastMoveTP = std::chrono::system_clock::now();
        }
        if (inputController->IsKeyHeld(VK_DOWN) || inputController->IsKeyPressed(VK_DOWN)) {
            position.Y++;
            lastMoveTP = std::chrono::system_clock::now();
        }
        if (inputController->IsKeyHeld(VK_LEFT) || inputController->IsKeyPressed(VK_LEFT)) {
            position.X--;
            lastMoveTP = std::chrono::system_clock::now();
        }
        if (inputController->IsKeyHeld(VK_RIGHT) || inputController->IsKeyPressed(VK_RIGHT)) {
            position.X++;
            lastMoveTP = std::chrono::system_clock::now();
        }
    }
}