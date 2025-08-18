#ifndef INETWORKDATAOBSERVER_H
#define INETWORKDATAOBSERVER_H

#include <string>
class INetworkDataObserver {
public:
	std::string NameReciever;
	virtual void HandleNetworkData(const std::string data) = 0;
};

#endif INETWORKDATAOBSERVER_H