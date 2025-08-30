#include "FPSMeter.h"

FPSMeter::FPSMeter() {
	lastTimePoint = std::chrono::system_clock::now();
}

float FPSMeter::Update() {
	std::chrono::system_clock::time_point currentTimePoint = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedTime = currentTimePoint - lastTimePoint;
	lastTimePoint = currentTimePoint;
	return 1 / elapsedTime.count(); // FPS = 1 / t, t = frame time
}