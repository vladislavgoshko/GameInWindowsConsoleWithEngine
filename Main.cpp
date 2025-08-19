#include <iostream>
#include <sstream>

#define NOMINMAX

#include "ConsoleEngine.h"
#include "TestObject.h"
#include "KeyboardInputController.h"
#include "Scene.h"
#include "Player.h"
#include "NetworkManager.h"
#include "FPSMeter.cpp"
#include "FrameCreator.h"
#include "LineDrawer.h"
//#pragma comment(lib, "Winmm.lib")

#include <fstream>

inline bool operator==(const COORD& lhs, const COORD& rhs) {
	return lhs.X == rhs.X && lhs.Y == rhs.Y;
}

inline bool operator!=(const COORD& lhs, const COORD& rhs) {
	return !(lhs == rhs);
}

// ���������� ������ ���� (3D)
struct Vec3 {
	float x, y, z;
};

// �������� 3D � 2D
COORD ProjectTo2D(const Vec3& v, int screenW, int screenH) {
	float fov = 200.0f; // "�����������"
	float z = v.z + 5.0f; // ������� ��� �����, ����� �� ������ �� ����
	return {
		(short)(screenW / 2 + (v.x * fov) / z),
		(short)(screenH / 2 + (v.y * fov) / z)
	};
}

int main()
{
	OutputDebugStringA("START DEBUG\n");

	ConsoleEngine ce;
	NetworkManager nm;

	KeyboardInputController inputController;

	std::vector<IDrawableObject*> objects;
	TestObject obj1({ 40, 20 }, 40, &inputController, &ce);
	objects.push_back(&obj1);

	COORD currentWindowSize = ce.GetWindowSize();
	FrameCreator frameCreator = FrameCreator(currentWindowSize);
	LineDrawer lineDrawer(&frameCreator);

	auto frameTimeRegulationTimePoint = std::chrono::system_clock::now(); // ��� ��������� ���
	double frameTime = 1. / 60; // ������ ����� �����

	//std::stringstream title("Game");
	ce.ChangeTitle("TermGame");

	std::stringstream debugInfo;

	std::chrono::duration<float> deltaTime = std::chrono::system_clock::now() - frameTimeRegulationTimePoint;

	FPSMeter fpsMeter;

	float angle = 0.0f;

	// � main():
	std::vector<Vec3> cubeVertices = {
		{-0.4f, -0.4f, -0.4f}, {0.4f, -0.4f, -0.4f}, {0.4f,  0.4f, -0.4f}, {-0.4f,  0.4f, -0.4f}, // ������ �����
		{-0.4f, -0.4f,  0.4f}, {0.4f, -0.4f,  0.4f}, {0.4f,  0.4f,  0.4f}, {-0.4f,  0.4f,  0.4f}  // �������� �����
	};

	// и��� ���� (������� ������)
	std::vector<std::pair<int, int>> cubeEdges = {
		{0,1},{1,2},{2,3},{3,0}, // ������ �����
		{4,5},{5,6},{6,7},{7,4}, // �������� �����
		{0,4},{1,5},{2,6},{3,7}  // ���������� ������
	};

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
			// �������� ������ � IUpdatable* � �������� Update
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

			angle += 0.03f; // �������� ��������
		}
		


		// ��������� ����
		// --- � ����� ��������� ---
		std::vector<COORD> projected;
		projected.reserve(cubeVertices.size());


		for (auto v : cubeVertices) {
			// �������� ������ Y
			float x1 = v.x * cosf(angle) - v.z * sinf(angle);
			float z1 = v.x * sinf(angle) + v.z * cosf(angle);
			float y1 = v.y;

			// �������� ������ X
			float y2 = y1 * cosf(angle * 0.7f) - z1 * sinf(angle * 0.7f);
			float z2 = y1 * sinf(angle * 0.7f) + z1 * cosf(angle * 0.7f);

			Vec3 rotated = { x1, y2, z2 };
			projected.push_back(ProjectTo2D(rotated, windowSize.X, windowSize.Y));
		}

		// ������ ����
		for (size_t e = 0; e < cubeEdges.size(); e++) {
			int i1 = cubeEdges[e].first;
			int i2 = cubeEdges[e].second;
			lineDrawer.DrawLine(projected[i1], projected[i2], 4);
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