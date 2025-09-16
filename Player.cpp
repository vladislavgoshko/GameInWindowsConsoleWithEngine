#include "Player.h"
#include <sstream>
#include <Windows.h>
#include "KeyboardInputController.h"
#include "Bullet.h"
#include "PhysicsEngine.h"
#include "MouseInputController.h"

Player::Player(std::string name, int health, float speed, COORD position, std::vector<std::vector<SHORT>> texture, KeyboardInputController* inputController)
	: GameObject(name), health(health), speed(speed), position(position), texture(texture), inputController(inputController) {

	audio.loadWav(L"shot.wav", shot);

	collider.position = Vector2(position.X, position.Y);
	collider.size = Vector2(10, 15); // Пример размера (ширина, высота)
	rigidbody.velocity = Vector2(0, 0);
	rigidbody.mass = 1.0f;
	rigidbody.isStatic = false;
}

Player::~Player() {
	std::stringstream ss;
	ss << GetName() << " (Player) destroyed\n";
	OutputDebugStringA(ss.str().c_str());
}

void Player::TakeDamage(int damage) {
	health -= damage;
	std::stringstream ss;
	ss << GetName() << " took " << damage << " damage, health now: " << health << "\n";
	OutputDebugStringA(ss.str().c_str());
}

void Player::SetPhysicsEngine(PhysicsEngine* physics) {
    this->physics = physics;
}

void Player::SetMouseController(MouseInputController* mouse) {
    mouseController = mouse;
}

void Player::Update(float deltaTime) {

	auto currentTP = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedMoveTime = currentTP - lastMoveTP;
	if (elapsedMoveTime.count() > 1.0f / speed) {
	}
	rigidbody.velocity = Vector2(0, 0);
	if (inputController->IsKeyHeld('W') || inputController->IsKeyPressed('W')) {
		//position.Y--;
		//rigidbody.velocity += Vector2(0, -1);
		rigidbody.velocity.y = -1 * speed;
		lastMoveTP = std::chrono::system_clock::now();
	}
	if (inputController->IsKeyHeld('S') || inputController->IsKeyPressed('S')) {
		//position.Y++;
		//rigidbody.velocity += Vector2(0, 1);
		rigidbody.velocity.y = 1 * speed;
		lastMoveTP = std::chrono::system_clock::now();
	}
	if (inputController->IsKeyHeld('A') || inputController->IsKeyPressed('A')) {
		//position.X--;
		//collider.position.x -= 10;
		//rigidbody.velocity += Vector2(-1, 0);
		rigidbody.velocity.x = -1 * speed;
		lastMoveTP = std::chrono::system_clock::now();
	}
	if (inputController->IsKeyHeld('D') || inputController->IsKeyPressed('D')) {
		//position.X++;
		//collider.position.x += 1;
		//rigidbody.velocity += Vector2(1, 0);
		rigidbody.velocity.x = 1 * speed;
		lastMoveTP = std::chrono::system_clock::now();
	}

	// Стрельба
	//if (bullets && (inputController->IsKeyPressed(VK_SPACE)) || inputController->IsKeyHeld(VK_SPACE)) {
	//	auto now = std::chrono::system_clock::now();
	//	std::chrono::duration<float> elapsed = now - lastShotTP;
	//	if (elapsed.count() > fireCooldown) {
	//		audio.play(shot, false, 0.2f);
	//		COORD bulletPos = position;
	//		bulletPos.Y += 0;
	//		bulletPos.X += 12;
	//		Bullet* newBullet = new Bullet(bulletPos);
	//		bullets->push_back(newBullet);
	//		if (physics) {
	//			physics->AddObject(&newBullet->rigidbody, &newBullet->collider, newBullet);
	//		}
	//		lastShotTP = now;
	//	}
	//}
	// Стрельба по ЛКМ
	if (bullets && mouseController) {
		auto state = mouseController->GetState();
		if (state.leftEvent == MouseInputController::ButtonEventType::Pressed || state.leftEvent == MouseInputController::ButtonEventType::Hold) {
			auto now = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsed = now - lastShotTP;
			if (elapsed.count() > fireCooldown) {
				audio.play(shot, false, 0.2f);
				// Центр игрока
				float px = collider.position.x + collider.size.x / 2.0f;
				float py = collider.position.y + collider.size.y / 2.0f;
				// Куда кликнули
				float tx = state.position.X;
				float ty = state.position.Y;
				ty *= 2; // потому что координата мыши в текстовом режиме в два раза меньше по Y
				// Вектор направления
				float dx = tx - px;
				float dy = ty - py;
				
				float len = sqrt(dx*dx + dy*dy);
				if (len > 0.01f) {
					dx /= len;
					dy /= len;
				}
				float bulletSpeed = 500.0f;
				COORD bulletPos = { (SHORT)px, (SHORT)py };
				Bullet* newBullet = new Bullet(bulletPos);
				newBullet->rigidbody.velocity = Vector2(dx * bulletSpeed, dy * bulletSpeed);
				bullets->push_back(newBullet);
				if (physics) {
					physics->AddObject(&newBullet->rigidbody, &newBullet->collider, newBullet, 2, 0b1000);
				}
				lastShotTP = now;
			}
		}
	}
	SyncPhysicsToPosition();
}

void Player::FixedUpdate(float fixedDeltaTime) {

	// auto currentTP = std::chrono::system_clock::now();
	// std::chrono::duration<float> elapsedMoveTime = currentTP - lastMoveTP;
	// rigidbody.velocity = Vector2(0, 0);
	// if (elapsedMoveTime.count() > 1.0f / speed) {
	//     if (inputController->IsKeyHeld(VK_UP) || inputController->IsKeyPressed(VK_UP)) {
	//         //position.Y--;
			 //rigidbody.velocity += Vector2(0, -50);
	//         lastMoveTP = std::chrono::system_clock::now();
	//     }
	//     if (inputController->IsKeyHeld(VK_DOWN) || inputController->IsKeyPressed(VK_DOWN)) {
	//         //position.Y++;
			 //rigidbody.velocity += Vector2(0, 50);
	//         lastMoveTP = std::chrono::system_clock::now();
	//     }
	//     if (inputController->IsKeyHeld(VK_LEFT) || inputController->IsKeyPressed(VK_LEFT)) {
	//         //position.X--;
			 ////collider.position.x -= 10;
			 //rigidbody.velocity += Vector2(-50, 0);
	//         lastMoveTP = std::chrono::system_clock::now();
	//     }
	//     if (inputController->IsKeyHeld(VK_RIGHT) || inputController->IsKeyPressed(VK_RIGHT)) {
	//         //position.X++;
	//         //collider.position.x += 1;
			 //rigidbody.velocity += Vector2(50, 0);
	//         lastMoveTP = std::chrono::system_clock::now();
	//     }
	// }
	SyncPositionToPhysics();
}

const std::vector<std::vector<SHORT>>& Player::GetTexture() const {
	return texture;
}

COORD Player::GetPosition() const {
	return position;
}

void Player::SetBulletContainer(std::vector<Bullet*>* bullets) {
	this->bullets = bullets;
}

void Player::SyncPhysicsToPosition() {
	//collider.position = Vector2(position.X, position.Y);
}

void Player::SyncPositionToPhysics() {
	position.X = static_cast<SHORT>(collider.position.x);
	position.Y = static_cast<SHORT>(collider.position.y);
}
