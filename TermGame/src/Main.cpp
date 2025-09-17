#define NOMINMAX
#define MINIAUDIO_IMPLEMENTATION

#include "core\ConsoleEngine.h"
#include "core\Scene.h"
#include "core\FPSMeter.h"
#include "core\FrameCreator.h"
#include "core\PhysicsEngine.h"

#include "input\MouseInputController.h"
#include "input\KeyboardInputController.h"
#include "input\MouseHelper.h"

#include "network\NetworkManager.h"

#include "objects\TestObject.h"
#include "objects\Player.h"
#include "objects\DrawableGameObject.h"
#include "objects\Bullet.h"
#include "objects\Wall.h"
#include "objects\RemotePlayer.h"

#include "utils\LineDrawer.h"

#include "audio\MiniWaveOutAudio.h"

#include <iomanip>
#include <sstream>

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
	// ������� �� 16 �������� ������
	std::vector<ColorRGB> grayPalette(16);
	for (int i = 0; i < 16; ++i) {
		BYTE gray = static_cast<BYTE>(i * 255 / 15);
		grayPalette[i] = { gray, gray, gray };
	}
	ce.SetColorPallete(grayPalette);

	KeyboardInputController inputController;
	MouseInputController mouse(ce.handleConsoleIn);
	std::vector<IDrawableObject*> objects;
	std::vector<Bullet*> bullets; // ��������� ��� ����
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
	PhysicsEngine physics;
	player.SetBulletContainer(&bullets); // ��������� ��������� ���� � �������
	player.SetPhysicsEngine(&physics); // ��������� ������ � �������
	player.SetMouseController(&mouse); // ��������� ���� � �������
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

	auto frameTimeRegulationTimePoint = std::chrono::system_clock::now(); // ��� ��������� ���
	float frameTime = 1.f / 60; // ������ ����� �����

	//std::stringstream title("Game");
	ce.ChangeTitle("TermGame");

	std::stringstream debugInfo;

	auto lastFrameTime = std::chrono::system_clock::now();
	std::chrono::duration<float> deltaTime = std::chrono::duration<float>(0);

	FPSMeter fpsMeter;


	MiniAudio::Engine audio; // 44100 Hz, stereo
	MiniAudio::WavData shot;
	MiniAudio::WavData bg;
	//audio.loadWav(L"shot.wav", shot);
	//audio.loadWav(L"KEYGEN CHURCH - La Chiave Del Mio Amor.wav", bg);
	//audio.play(bg, true, 0.5f);

	physics.AddObject(&player.rigidbody, &player.collider, &player, 1, 0b1000);

	// --- ���������� ����� ---
	Wall* wall = new Wall({ 50, 20 }, { 20, 2 }, 8); // ������� (40,10), ������ 10x5, ���� 8
	Wall* wall1 = new Wall({ 70, 20 }, { 2, 20 }, 8); // ������� (40,10), ������ 10x5, ���� 8
	Wall* wall2 = new Wall({ 50, 40 }, { 20, 2 }, 8); // ������� (40,10), ������ 10x5, ���� 8
	Wall* wall3 = new Wall({ 30, 20 }, { 10, 5 }, 8); // ������� (40,10), ������ 10x5, ���� 8
	physics.AddObject(nullptr, &wall->collider, wall, 3, 0); // ����� ��������, ��� Rigidbody
	physics.AddObject(nullptr, &wall1->collider, wall1, 3, 0); // ����� ��������, ��� Rigidbody
	physics.AddObject(nullptr, &wall2->collider, wall2, 3, 0); // ����� ��������, ��� Rigidbody
	physics.AddObject(nullptr, &wall3->collider, wall3, 3, 0); // ����� ��������, ��� Rigidbody
	objects.push_back(wall);
	objects.push_back(wall1);
	objects.push_back(wall2);
	objects.push_back(wall3);
	//ce.SwitchFullscreen(true);

	// Networking setup
	NetworkManager net;
	char mode = 's';
	std::cout << "Run as (s)erver or (c)lient? ";
	std::cin >> mode;
	RemotePlayer* remote = nullptr;
	if (mode == 's') {
		net.StartAcceptConnections(12345);
		// create remote player placeholder
		remote = new RemotePlayer("Remote", 100, 30, {40,20}, playerTexture);
		remote->SetBulletContainer(&bullets);
		remote->SetPhysicsEngine(&physics);
		objects.push_back(remote);
		net.AddObserver(remote);
	} else {
		std::string ip;
		std::cout << "Server IP: ";
		//std::cin >> ip;
		ip = "127.0.0.1";
		if (net.ConnectToServer(ip, 12345, "local_player")) {
			player.SetBindings({ VK_UP,VK_DOWN,VK_LEFT,VK_RIGHT,VK_SPACE });
			remote = new RemotePlayer("Remote", 100, 30, {40,20}, playerTexture);
			remote->SetBulletContainer(&bullets);
			remote->SetPhysicsEngine(&physics);
			objects.push_back(remote);
			net.AddObserver(remote);
		} else {
			std::cerr << "Failed to connect to server" << std::endl;
		}
	}

	while (true) {
		auto now = std::chrono::system_clock::now();
		deltaTime = now - lastFrameTime;
		lastFrameTime = now;


		debugInfo.str("");
		auto windowSize = ce.GetWindowSize();
		debugInfo << std::fixed << std::setprecision(2) << std::setw(4);
		debugInfo << "Sizes(" << windowSize.X << "," << windowSize.Y << ") "
			//<< deltaTime.count() 
			//<< " " << frameTime 
			<< " " << ce.IsFullcreen
			<< " " << fpsMeter.Update()
			;

		// UPDATE
		physics.Update(deltaTime.count());

		inputController.Update();
		for (auto obj : objects) {
			// �������� ������ � IUpdatable* � �������� Update
			if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
				updatable->Update(deltaTime.count());
			}
		}
		// ���������� ����
		for (auto bullet : bullets) {
			bullet->Update(deltaTime.count());
		}
		// --- ������ ---
		//for (auto bullet : bullets) {
		//    physics.AddObject(&bullet->rigidbody, &bullet->collider, bullet);
		//}
		//physics.Update(deltaTime.count());

		// FIXED UPDATE
		if (now - frameTimeRegulationTimePoint > std::chrono::duration<float>(frameTime)) {
			frameTimeRegulationTimePoint = std::chrono::system_clock::now();

			for (auto obj : objects) {
				if (IUpdatable* updatable = dynamic_cast<IUpdatable*>(obj)) {
					updatable->FixedUpdate(frameTime);
				}
			}
			for (auto bullet : bullets) {
				bullet->FixedUpdate(frameTime);
			}
		}

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
		// ������ ����
		for (auto it = bullets.begin(); it != bullets.end();) {
			if ((*it)->IsOutOfBounds({ frameSize.X,  (SHORT)(frameSize.Y * 2) }) || (*it)->IsDestroyed()) {
				physics.RemoveObject(*it); // ������� �� PhysicsEngine
				delete* it;
				it = bullets.erase(it);
			}
			else {
				frameCreator.DrawObject(**it);
				++it;
			}
		}



		mouse.Update();
		auto state = mouse.GetState();

		// �������: ������ ����� ������
		if (state.leftEvent == MouseInputController::ButtonEventType::Hold) {
			std::stringstream ss;
			ss << "Left button PRESSED at X=" << state.position.X
				<< " Y=" << state.position.Y << "\n";
			OutputDebugStringA(ss.str().c_str());
			frameCreator.DrawPixel(state.position.X, state.position.Y * 2, 10);
		}

		// �������: �������� ����� ������
		/*if (state.leftEvent == MouseInputController::ButtonEventType::Released) {
			std::stringstream ss;
			ss << "Left button RELEASED at X=" << state.position.X
				<< " Y=" << state.position.Y << "\n";
			OutputDebugStringA(ss.str().c_str());
		}*/

		// ������
		/*if (mouse.IsWheelScrolledUp()) {
			OutputDebugStringA("Wheel UP\n");
		}
		if (mouse.IsWheelScrolledDown()) {
			OutputDebugStringA("Wheel DOWN\n");
		}*/

		// ������� ����
		/*if (mouse.IsDoubleClick()) {
			OutputDebugStringA("Double click\n");
		}*/

		//if (mouse.IsLeftButtonPressed()) {
		//	auto state = mouse.GetState();

		//	bool upper = MouseHelper::IsUpperHalf(state.position);
		//	if (upper)
		//		OutputDebugStringA("Click in UPPER half of char\n");
		//	else
		//		OutputDebugStringA("Click in LOWER half of char\n");
		//}
		frameCreator.WriteText({ 3, 2 }, "MENU", 0, 4);
		frameCreator.WriteText({ 3, 4 }, "Start");

		POINT cursorPos;
		GetCursorPos(&cursorPos);
		debugInfo << " cursorPos(" << cursorPos.x << "," << cursorPos.y << ") ";
		debugInfo << " charPos(" << state.position.X << "," << state.position.Y << ") ";
		if (inputController.IsKeyPressed(27))
			return 0;


		frameCreator.WriteText({ 0, 0 }, debugInfo.str(), 0, 15);

		// networking: send local player position periodically
		if (remote) {
			std::stringstream ss;
			ss << "POS " << player.GetPosition().X << " " << player.GetPosition().Y;
			if (mode == 's') {
				net.BroadcastData(ss.str()); // ������ ��������� ����
			} else {
				auto tmpSS = ss.str();
				net.SendData(tmpSS, "server"); // ������ ���������� �������
			}
		}

		try {
			ce.Draw(frameCreator.GetFrame().data());
		}
		catch (...) {

		}
		//frameCreator.ClearFrame();
		/*while (ce.GetWindowSize().X < 200) {
			ce.DecreaseScale();
			Sleep(100);
		}
		while (ce.GetWindowSize().X > 300) {
			ce.IncreaseScale();
			Sleep(100);
		}*/

	}
	return 0;
}