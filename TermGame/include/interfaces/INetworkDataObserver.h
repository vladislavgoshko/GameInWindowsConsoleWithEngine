#pragma once

#include <string>

class INetworkDataObserver {
public:
	std::string NameReciever;
	virtual void HandleNetworkData(const std::string data) = 0;
};