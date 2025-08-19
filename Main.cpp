#include <iostream>
#include <sstream>
#include "ConsoleEngine.h"
#include "TestObject.h"
#include "KeyboardInputController.h"
#include "Scene.h"
#include "Player.h"
#include "NetworkManager.h"
#include "FrameCreator.h"
//#pragma comment(lib, "Winmm.lib")

#include <fstream>
#include "FPSMeter.cpp"

inline bool operator==(const COORD& lhs, const COORD& rhs) {
	return lhs.X == rhs.X && lhs.Y == rhs.Y;
}

inline bool operator!=(const COORD& lhs, const COORD& rhs) {
	return !(lhs == rhs);
}

int main()
{
	OutputDebugStringA("START DEBUG\n");

	ConsoleEngine ce;
	NetworkManager nm;

	KeyboardInputController inputController;

	std::vector<IDrawableObject*> objects;
	TestObject obj1({ 5, 5 }, 40, &inputController, &ce);
	objects.push_back(&obj1);

	COORD currentWindowSize = ce.GetWindowSize();
	FrameCreator frameCreator = FrameCreator(currentWindowSize);


	auto frameTimeRegulationTimePoint = std::chrono::system_clock::now(); // Для регуляции фпс
	double frameTime = 1. / 60; // Нужная длина кадра

	//std::stringstream title("Game");
	ce.ChangeTitle("TermGame");

	std::stringstream debugInfo;

	std::chrono::duration<float> deltaTime = std::chrono::system_clock::now() - frameTimeRegulationTimePoint;

	FPSMeter fpsMeter;

	/*std::ofstream outfile("data.txt");
	if (!outfile) {
		std::cerr << "Ошибка открытия файла!" << std::endl;
		return 1;
	}*/
	while (true) {
		//fps = fpsMeter.Update();

		deltaTime = std::chrono::system_clock::now() - frameTimeRegulationTimePoint;

		frameCreator.WriteText({ 3, 2 }, "MENU", 0, 11);
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
		frameCreator.WriteText({ 0, 0 }, debugInfo.str(), 0, 15);

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
			frameTimeRegulationTimePoint = std::chrono::system_clock::now();

			for (auto obj : objects) {
				if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
					updatable->FixedUpdate(frameTime);
				}
			}
		}

		windowSize = ce.GetWindowSize();
		COORD frameSize = frameCreator.GetFrameSize();
		if (windowSize != frameSize) {
			frameCreator.Resize(windowSize);
		}

		for (auto& obj : objects) {
			frameCreator.DrawObject(*obj);
		}
		ce.Draw(frameCreator.GetFrame().data());
		frameCreator.ClearFrame();
	}

	return 0;
}