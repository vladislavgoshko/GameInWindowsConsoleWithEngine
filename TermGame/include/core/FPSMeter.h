#pragma once
#include <chrono>

class FPSMeter {
public:
	FPSMeter();
	float Update();

private:
	std::chrono::system_clock::time_point lastTimePoint;
};