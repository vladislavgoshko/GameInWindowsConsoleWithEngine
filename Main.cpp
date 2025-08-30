#define NOMINMAX

#include "ConsoleEngine.h"
#include "TestObject.h"
#include "KeyboardInputController.h"
#include "Scene.h"
#include "Player.h"
#include "NetworkManager.h"
#include "FPSMeter.h"
#include "FrameCreator.h"
#include "LineDrawer.h"
#include "DrawableGameObject.h"
#include "Bullet.h"
#include "PhysicsEngine.h"
#include "MouseInputController.h"
#include "MouseHelper.h"
#include "Wall.h"
//#pragma comment(lib, "Winmm.lib")
#define MINIAUDIO_IMPLEMENTATION
#include "MiniWaveOutAudio.h"
//#include <fstream>

inline bool operator==(const COORD& lhs, const COORD& rhs) {
	return lhs.X == rhs.X && lhs.Y == rhs.Y;
}

inline bool operator!=(const COORD& lhs, const COORD& rhs) {
	return !(lhs == rhs);
}

// Координаты вершин куба (3D)
struct Vec3 {
	float x, y, z;
};

// Проекция 3D в 2D
COORD ProjectTo2D(const Vec3& v, int screenW, int screenH) {
	float fov = 200.0f; // "приближение"
	float z = v.z + 5.0f; // смещаем куб вперёд, чтобы не делить на ноль
	return {
		(short)(screenW / 2 + (v.x * fov) / z),
		(short)(screenH / 2 + (v.y * fov) / z)
	};
}

int main()
{
	OutputDebugStringA("START DEBUG\n");

	ConsoleEngine ce;
	//NetworkManager nm;

	KeyboardInputController inputController;

	std::vector<IDrawableObject*> objects;
	std::vector<Bullet*> bullets; // Контейнер для пуль
	TestObject obj1({ 40, 20 }, 40, &inputController, &ce);

	std::vector<std::vector<SHORT>>  playerTexture = {
	{-1, -1, -1, 4, 4, 4},
	{-1, -1, -1, 4, 4, 4},
	{-1, -1, -1, 4, 4, 4},
	{},
	{4, -1, 4, 4, 4, 4, 4, 4, -1, 4},
	{4, -1, 4, 4, 4, 4, 4, 4, -1, 4},
	{4, -1, 4, 4, 4, 4, 4, 4, -1, 4},
	{4, -1, 4, 4, 4, 4, 4, 4, -1, 4},
	{4, -1, 4, 4, 4, 4, 4, 4, -1, 4},
	{-1, -1, 4, 4, 4, 4, 4, 4, -1, -1},
	{},
	{-1, -1, -1, 4, -1, -1, 4},
	{-1, -1, -1, 4, -1, -1, 4},
	{-1, -1, -1, 4, -1, -1, 4},
	{-1, -1, -1, 4, -1, -1, 4},
	};
	std::vector<std::vector<SHORT>> carTexture = {
		{-1, 4, 3, 3},
		{4, 4, 4, 4, 4, 4},
		{-1, 0, -1, -1, 0}
	};
	std::vector<std::vector<SHORT>> treeTexture = {
		{-1, -1, -1, 2, 2, 2, -1},
		{-1, -1, 2, 2, 2, 2, 2},
		{-1, -1, 2, 2, 2, 2, 2},
		{-1, -1, 2, 2, 2, 2, 2},
		{-1, -1, -1, 2, 2, 2, -1},
		{2, 2, -1, -1, 12, -1, -1},
		{2, 2, 12, 12, 12, -1, -1},
		{-1, -1, -1, 12, -1, -1, -1},
		{-1, -1, -1, 12, -1, -1, -1}
	};
	std::vector<std::vector<SHORT>> backgroundTexture(1000, std::vector<SHORT>(300, 15));

	Player player("Player", 100, 30, {20, 20}, playerTexture, &inputController);
	player.SetBulletContainer(&bullets); // Связываем контейнер пуль с игроком
	DrawableGameObject background("Background", { 0, 0 }, backgroundTexture);
	DrawableGameObject car("Car", { 24, 20 }, carTexture);
	DrawableGameObject tree("Tree", { 20, 32 }, treeTexture);

	objects.push_back(&background);
	objects.push_back(&player);
	objects.push_back(&car);
	objects.push_back(&tree);
	objects.push_back(&obj1);

	COORD currentWindowSize = ce.GetWindowSize();
	FrameCreator frameCreator = FrameCreator(currentWindowSize);
	LineDrawer lineDrawer(&frameCreator);

	auto frameTimeRegulationTimePoint = std::chrono::system_clock::now(); // Для регуляции фпс
	float frameTime = 1.f / 60; // Нужная длина кадра

	//std::stringstream title("Game");
	ce.ChangeTitle("TermGame");

	std::stringstream debugInfo;

	std::chrono::duration<float> deltaTime = std::chrono::system_clock::now() - frameTimeRegulationTimePoint;

	FPSMeter fpsMeter;

	float angle = 0.0f;

	// В main():
	std::vector<Vec3> cubeVertices = {
		{-0.4f, -0.4f, -0.4f}, {0.4f, -0.4f, -0.4f}, {0.4f,  0.4f, -0.4f}, {-0.4f,  0.4f, -0.4f}, // задняя грань
		{-0.4f, -0.4f,  0.4f}, {0.4f, -0.4f,  0.4f}, {0.4f,  0.4f,  0.4f}, {-0.4f,  0.4f,  0.4f}  // передняя грань
	};

	// Рёбра куба (индексы вершин)
	std::vector<std::pair<int, int>> cubeEdges = {
		{0,1},{1,2},{2,3},{3,0}, // задняя грань
		{4,5},{5,6},{6,7},{7,4}, // передняя грань
		{0,4},{1,5},{2,6},{3,7}  // соединение граней
	};

	INPUT_RECORD inputRecord;
	DWORD eventsRead;
	DWORD eventsAvailable = 0;
	MouseInputController mouse(ce.handleConsoleIn); 


	MiniAudio::Engine audio; // 44100 Hz, stereo
	MiniAudio::WavData shot;
	MiniAudio::WavData bg;
	//audio.loadWav(L"shot.wav", shot);
	//audio.loadWav(L"KEYGEN CHURCH - La Chiave Del Mio Amor.wav", bg);
	//audio.play(bg, true, 0.5f);

	PhysicsEngine physics;
	physics.AddObject(&player.rigidbody, &player.collider);
	
    // --- Добавление стены ---
    Wall* wall = new Wall({50, 20}, {10, 5}, 8); // позиция (40,10), размер 10x5, цвет 8
    physics.AddObject(nullptr, &wall->collider, wall); // стена статична, без Rigidbody
    objects.push_back(wall);

	while (true) {
		//fps = fpsMeter.Update();


		frameCreator.WriteText({ 3, 2 }, "MENU", 0, 4);
		frameCreator.WriteText({ 3, 4 }, "Start");
		//ce.WriteText({ 3, 5 }, "Settings");
		//ce.WriteText({ 3, 6 }, "Exit");

		debugInfo.str("");
		auto windowSize = ce.GetWindowSize();

		debugInfo << "Sizes(" << windowSize.X << "," << windowSize.Y << ") "
			//<< deltaTime.count() 
			//<< " " << frameTime 
			<< " " << ce.IsFullcreen
			<< " " << fpsMeter.Update()
			;

		// UPDATE
		inputController.Update();
		for (auto obj : objects) {
			// Приводим объект к IUpdatable* и вызываем Update
			if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
				updatable->Update(deltaTime.count());
			}
		}
		// Обновление пуль
		for (auto bullet : bullets) {
			bullet->Update(deltaTime.count());
		}
		// --- Физика ---
		//physics.Update(deltaTime.count());

		// FIXED UPDATE
		if (deltaTime > std::chrono::duration<float>(frameTime)) {
			frameTimeRegulationTimePoint = std::chrono::system_clock::now();

			for (auto obj : objects) {
				if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
					updatable->FixedUpdate(frameTime);
				}
			}
			for (auto bullet : bullets) {
				bullet->FixedUpdate(frameTime);
			}

			

			angle += 0.03f; // скорость вращения
		}
		physics.Update(deltaTime.count());

		windowSize = ce.GetWindowSize();
		COORD frameSize = frameCreator.GetWindowSize();
		if (windowSize != frameSize) {
			frameCreator.Resize(windowSize);
		}

		for (auto& obj : objects) {
			frameCreator.DrawObject(*obj);
		}
		for (int color = 0; color < 16; ++color) {
			frameCreator.DrawPixel(2 + color, 30, color);
		}
		// Рисуем пули
		for (auto it = bullets.begin(); it != bullets.end();) {
			if ((*it)->IsOutOfBounds({ frameSize.X,  (SHORT)(frameSize.Y * 2) })) {
				delete* it;
				it = bullets.erase(it);
			}
			else {
				frameCreator.DrawObject(**it);
				++it;
			}
		}
		// РИСОВАНИЕ КУБА
		// --- в цикле рисования ---
		std::vector<COORD> projected;
		projected.reserve(cubeVertices.size());

		for (auto v : cubeVertices) {
			// вращение вокруг Y
			float x1 = v.x * cosf(angle) - v.z * sinf(angle);
			float z1 = v.x * sinf(angle) + v.z * cosf(angle);
			float y1 = v.y;

			// вращение вокруг X
			float y2 = y1 * cosf(angle * 0.7f) - z1 * sinf(angle * 0.7f);
			float z2 = y1 * sinf(angle * 0.7f) + z1 * cosf(angle * 0.7f);

			Vec3 rotated = { x1, y2, z2 };
			projected.push_back(ProjectTo2D(rotated, windowSize.X, windowSize.Y));
		}

		// рисуем рёбра
		for (size_t e = 0; e < cubeEdges.size(); e++) {
			int i1 = cubeEdges[e].first;
			int i2 = cubeEdges[e].second;
			lineDrawer.DrawLine(projected[i1], projected[i2], 4);
		}
		frameCreator.WriteText({ 0, 0 }, debugInfo.str(), 0, 15);

		
	
		mouse.Update();
		auto state = mouse.GetState();

		// событие: нажата левая кнопка
		if (state.leftEvent == MouseInputController::ButtonEventType::Hold) {
			std::stringstream ss;
			ss << "Left button PRESSED at X=" << state.position.X
				<< " Y=" << state.position.Y << "\n";
			OutputDebugStringA(ss.str().c_str());
			frameCreator.DrawPixel(state.position.X, state.position.Y * 2, 10);
		}

		// событие: отпущена левая кнопка
		if (state.leftEvent == MouseInputController::ButtonEventType::Released) {
			std::stringstream ss;
			ss << "Left button RELEASED at X=" << state.position.X
				<< " Y=" << state.position.Y << "\n";
			OutputDebugStringA(ss.str().c_str());
		}

		// колесо
		if (mouse.IsWheelScrolledUp()) {
			OutputDebugStringA("Wheel UP\n");
		}
		if (mouse.IsWheelScrolledDown()) {
			OutputDebugStringA("Wheel DOWN\n");
		}

		// двойной клик
		if (mouse.IsDoubleClick()) {
			OutputDebugStringA("Double click\n");
		}

		//if (mouse.IsLeftButtonPressed()) {
		//	auto state = mouse.GetState();

		//	bool upper = MouseHelper::IsUpperHalf(state.position);
		//	if (upper)
		//		OutputDebugStringA("Click in UPPER half of char\n");
		//	else
		//		OutputDebugStringA("Click in LOWER half of char\n");
		//}

		ce.Draw(frameCreator.GetFrame().data());
		//frameCreator.ClearFrame();
		deltaTime = std::chrono::system_clock::now() - frameTimeRegulationTimePoint;

	}
	return 0;
}