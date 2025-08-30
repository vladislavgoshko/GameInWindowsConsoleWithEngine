#include "Player.h"
#include <sstream>
#include <Windows.h>
#include "KeyboardInputController.h"
#include "Bullet.h"
#include "PhysicsEngine.h"

Player::Player(std::string name, int health, float speed, COORD position, std::vector<std::vector<SHORT>> texture, KeyboardInputController* inputController)
	: GameObject(name), health(health), speed(speed), position(position), texture(texture), inputController(inputController) {

	audio.loadWav(L"Untitled video - Made with Clipchamp.wav", shot);

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

void Player::Update(float deltaTime) {

	auto currentTP = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedMoveTime = currentTP - lastMoveTP;
	if (elapsedMoveTime.count() > 1.0f / speed) {
	}
	rigidbody.velocity = Vector2(0, 0);
	if (inputController->IsKeyHeld(VK_UP) || inputController->IsKeyPressed(VK_UP)) {
		//position.Y--;
		//rigidbody.velocity += Vector2(0, -1);
		rigidbody.velocity.y = -1 * speed;
		lastMoveTP = std::chrono::system_clock::now();
	}
	if (inputController->IsKeyHeld(VK_DOWN) || inputController->IsKeyPressed(VK_DOWN)) {
		//position.Y++;
		//rigidbody.velocity += Vector2(0, 1);
		rigidbody.velocity.y = 1 * speed;
		lastMoveTP = std::chrono::system_clock::now();
	}
	if (inputController->IsKeyHeld(VK_LEFT) || inputController->IsKeyPressed(VK_LEFT)) {
		//position.X--;
		//collider.position.x -= 10;
		//rigidbody.velocity += Vector2(-1, 0);
		rigidbody.velocity.x = -1 * speed;
		lastMoveTP = std::chrono::system_clock::now();
	}
	if (inputController->IsKeyHeld(VK_RIGHT) || inputController->IsKeyPressed(VK_RIGHT)) {
		//position.X++;
		//collider.position.x += 1;
		//rigidbody.velocity += Vector2(1, 0);
		rigidbody.velocity.x = 1 * speed;
		lastMoveTP = std::chrono::system_clock::now();
	}

	// Стрельба
	if (bullets && (inputController->IsKeyPressed(VK_SPACE)) || inputController->IsKeyHeld(VK_SPACE)) {
		auto now = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsed = now - lastShotTP;
		if (elapsed.count() > fireCooldown) {
			audio.play(shot, false, 0.2f);
			COORD bulletPos = position;
			bulletPos.Y += 0;
			bulletPos.X += 12;
			Bullet* newBullet = new Bullet(bulletPos);
			bullets->push_back(newBullet);
			if (physics) {
				physics->AddObject(&newBullet->rigidbody, &newBullet->collider, newBullet);
			}
			lastShotTP = now;
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
