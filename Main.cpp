#include <iostream>
#include <sstream>
#include "ConsoleEngine.h"
#include "TestObject.h"
#include "KeyboardInputController.h"
#include "Scene.h"
#include "Player.h"
#include "NetworkManager.h"
//#pragma comment(lib, "Winmm.lib")

#include <fstream>
#include "FPSMeter.cpp"


int main()
{
	OutputDebugStringA("START DEBUG\n");
	//Scene scene("TestScene");
	//Sleep(1000);
	//GameObject* go1 = new GameObject("TestObject1");
	//GameObject* go2 = new GameObject("TestObject2");
	//GameObject* go3 = new GameObject("TestObject3");
	//Sleep(1000);
	//// ��������� ������� � �����
	//scene.AddObject(*go1);
	//scene.AddObject(*go2);

	//go1->Destroy();
	//go2->Destroy();
	//go3->Destroy();

	ConsoleEngine ce;
	NetworkManager nm;

	KeyboardInputController inputController;

	std::vector<IDrawableObject*> objects;
	TestObject obj1({ 5, 5 }, 20, &inputController, &ce);
	//TestObject obj2({ 20, 10 }, 10, &inputController);
	objects.push_back(&obj1);
	//objects.push_back(&obj2);

	//std::vector<ColorRGB> colors(16);

	//// ���������� ������� colors
	//for (int i = 0; i < 16; ++i) {
	//	colors[i].Red = i * 256 / 16;
	//	colors[i].Green = i * 256 / 16;
	//	colors[i].Blue = i * 256 / 16;
	//}

	std::vector<ColorRGB> colors(16);

	for (int i = 0; i < 16; ++i) {
		float t = static_cast<float>(i) / 15.0f; // �� 0 �� 1
		colors[i].Red = static_cast<BYTE>(std::sin(t * 6.28318f + 0.0f) * 127 + 128);
		colors[i].Green = static_cast<BYTE>(std::sin(t * 6.28318f + 2.094f) * 127 + 128);
		colors[i].Blue = static_cast<BYTE>(std::sin(t * 6.28318f + 4.188f) * 127 + 128);
	}

	ce.SetColorPallete(colors);


	ce.ChangeConsoleColor(15, 2);
	//ge.ChangeConsoleColor(7, 10);
	//ge.ChangeConsoleColor(14, 6);

	auto frameTimeRegulationTimePoint = std::chrono::system_clock::now(); // ��� ��������� ���
	double frameTime = 1. / 60; // ������ ����� �����

	//std::stringstream title("Game");
	ce.ChangeTitle("TermGame");

	std::stringstream debugInfo;

	std::chrono::duration<float> deltaTime = std::chrono::system_clock::now() - frameTimeRegulationTimePoint;
	
	FPSMeter fpsMeter;
	float fps;

	/*std::ofstream outfile("data.txt");
	if (!outfile) {
		std::cerr << "������ �������� �����!" << std::endl;
		return 1;
	}*/
	while (true) {
		//fps = fpsMeter.Update();

		deltaTime = std::chrono::system_clock::now() - frameTimeRegulationTimePoint;

		ce.WriteText({ 3, 2 }, "MENU");
		ce.WriteText({ 3, 4 }, "Start");
		ce.WriteText({ 3, 5 }, "Settings");
		ce.WriteText({ 3, 6 }, "Exit");

		debugInfo.str("");
		auto windowSize = ce.GetWindowSize();

		debugInfo << "Sizes(" << windowSize.X << "," << windowSize.Y << ") "
			//<< deltaTime.count() 
			//<< " " << frameTime 
			<< " " << ce.IsFullcreen
			<< " " << fpsMeter.Update()
			;
		ce.WriteDebugInfo(debugInfo.str());

		// UPDATE
		inputController.Update();
		for (auto obj : objects) {
			// �������� ������ � IUpdatable* � �������� Update
			if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
				updatable->Update(deltaTime.count());
			}
		}


		// FIXED UPDATE
		if (deltaTime > std::chrono::duration<float>(frameTime)) {
			frameTimeRegulationTimePoint = std::chrono::system_clock::now();

			for (auto obj : objects) {
				// �������� ������ � IUpdatable* � �������� Update
				if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
					updatable->FixedUpdate(frameTime);
				}
			}
			//obj1.SetPosition({ (short)((short)fps % 100 + 10), (short)((short)fps % 60 + 2) });
		}

		//outfile << fps << std::endl; // ������ � ����
		ce.DrawObjects(objects);
	}

	return 0;
}