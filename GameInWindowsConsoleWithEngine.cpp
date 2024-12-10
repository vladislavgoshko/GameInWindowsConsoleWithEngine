#include <iostream>
#include <sstream>
#include "GraphicEngine.h"
#include "TestObject.h"
#include "KeyboardInputController.h"
#include "Scene.h"
#include "Player.h"
//#pragma comment(lib, "Winmm.lib")

float FPS(std::chrono::system_clock::time_point& tp1, std::chrono::system_clock::time_point& tp2) {
	tp2 = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = tp2 - tp1;
	tp1 = tp2;
	return 1 / elapsedTime.count(); // FPS
}

int main()
{
	OutputDebugStringA("Start debug\n");
	Scene scene("TestScene");
	GameObject go("testObject");
	auto object = std::make_unique<Player>("sds");
	GameObject* rawPointer = object.get(); // Сохраняем "сырой" указатель для теста
	scene.AddObject(std::move(object));

	// Удаляем объект
	rawPointer->Destroy();


	GraphicEngine ge;

	KeyboardInputController inputController;

	std::vector<IDrawableObject*> objects;
	TestObject obj1({ 5, 5 }, 5, &inputController);
	TestObject obj2({ 20, 10 }, 10, &inputController);
	objects.push_back(&obj1);
	objects.push_back(&obj2);

	auto tp1 = std::chrono::system_clock::now(); // Начало отсчета кадра
	auto tp2 = std::chrono::system_clock::now(); // Конец отсчета кадра
	
	//std::vector<ColorRGB> colors(16);

	//// Заполнение вектора colors
	//for (int i = 0; i < 16; ++i) {
	//	colors[i].Red = i * 256 / 16;
	//	colors[i].Green = i * 256 / 16;
	//	colors[i].Blue = i * 256 / 16;
	//}
	//ge.SetColorPallete(colors);


	ge.ChangeConsoleColor(7, 4);

	auto tpRegFPS = std::chrono::system_clock::now(); // Для регуляции фпс
	double frameTime = 1. / 60; // Длина кадра

	std::stringstream title;
	std::stringstream debugInfo;


	while (true) {
		float fps = FPS(tp1, tp2); // FPS
		std::chrono::duration<float> deltaTime = std::chrono::system_clock::now() - tpRegFPS;

		title.str("");
		title << fps;
		ge.ChangeTitle(title.str());

		debugInfo.str("");
		auto windowSize = ge.GetWindowSize();

		debugInfo << "Sizes(" << windowSize.X << "," << windowSize.Y << ") "
			<< deltaTime.count() << " " << frameTime << " HELLO";
		ge.WriteDebugIndo(debugInfo.str());

		// UPDATE
		inputController.Update();
		for (auto obj : objects) {
			// Приводим объект к IUpdatable* и вызываем Update
			if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
				updatable->Update(deltaTime.count());
			}
		}


		// FIXED UPDATE
		if (deltaTime > std::chrono::duration<float>(frameTime)) {
			tpRegFPS = std::chrono::system_clock::now();

			for (auto obj : objects) {
				// Приводим объект к IUpdatable* и вызываем Update
				if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
					updatable->FixedUpdate(frameTime);
				}
			}
			//obj1.SetPosition({ (short)((short)fps % 100 + 10), (short)((short)fps % 60 + 2) });
		}


		ge.DrawObjects(objects);
	}

	return 0;
}