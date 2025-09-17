#include "core\ConsoleEngine.h"
#include "input\KeyboardInputController.h"
#include "objects\TestObject.h"

TestObject::TestObject(COORD startPos, float playerSpeed, KeyboardInputController* inputCtrl, ConsoleEngine* ce)
    : position(startPos), speed(playerSpeed), inputController(inputCtrl), ce(ce), lastMoveTP(std::chrono::system_clock::now()) {

    texture = {
    {-1,-1,-1,-1,6,6,6,6,6,-1,-1,-1,-1},      // волосы
    {-1,-1,-1,6,14,14,14,14,6,-1,-1,-1,-1},  // голова
    {-1,-1,6,14,14,14,14,14,14,6,-1,-1,-1},
    {-1,-1,6,14,14,14,14,14,14,6,-1,-1,-1},
    {-1,-1,-1,-1,-1,14,14,-1,-1,-1,-1,-1,-1},    // шея
    {-1,-1,1,1,1,1,1,1,1,-1,-1,-1,-1},      // торс
    {-1,14,1,1,1,1,1,1,1,1,14,-1,-1},    // руки
    {-1,14,1,1,1,4,4,1,1,1,14,-1,-1},    // ремень
    {-1,-1,1,1,1,1,1,1,1,-1,-1,-1,-1},
    {-1,-1,8,8,8,8,8,8,8,-1,-1,-1,-1},      // штаны
    {-1,-1,8,8,8,-1,-1,8,8,-1,-1,-1,-1},      // ноги
    {-1,-1,8,8,8,-1,-1,8,8,-1,-1,-1,-1},
    {-1,-1,3,3,-1,-1,-1,-1,3,3,-1,-1,-1}       // обувь
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
    //if (inputController->IsKeyPressed(VK_SPACE)) {
    //    ce->SwitchFullscreen(!ce->IsFullcreen);
    //}
}

//void TestObject::FixedUpdate(float deltaTime) {
//    auto currentTP = std::chrono::system_clock::now();
//    std::chrono::duration<float> elapsedMoveTime = currentTP - lastMoveTP;
//
//    if (elapsedMoveTime.count() > 1.0f / speed) {
//        if (inputController->IsKeyHeld(VK_UP) || inputController->IsKeyPressed(VK_UP)) {
//            position.Y--;
//            lastMoveTP = std::chrono::system_clock::now();
//        }
//        if (inputController->IsKeyHeld(VK_DOWN) || inputController->IsKeyPressed(VK_DOWN)) {
//            position.Y++;
//            lastMoveTP = std::chrono::system_clock::now();
//        }
//        if (inputController->IsKeyHeld(VK_LEFT) || inputController->IsKeyPressed(VK_LEFT)) {
//            position.X--;
//            lastMoveTP = std::chrono::system_clock::now();
//        }
//        if (inputController->IsKeyHeld(VK_RIGHT) || inputController->IsKeyPressed(VK_RIGHT)) {
//            position.X++;
//            lastMoveTP = std::chrono::system_clock::now();
//        }
//    }
//}

void TestObject::FixedUpdate(float deltaTime) {
    static COORD startPos = position;   // центр траектории
    static float angle = 0.0f;          // текущий угол в радианах
    float radius = 20.0f;               // радиус "движения"

    // скорость вращения (чем больше, тем быстрее крутится)
    float angularSpeed = 1.0f; // радиан в секунду

    angle += angularSpeed * deltaTime;

    position.X = startPos.X + static_cast<SHORT>(std::cos(angle) * radius);
    position.Y = startPos.Y + static_cast<SHORT>(std::sin(angle) * radius);
}
