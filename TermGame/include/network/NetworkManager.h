#pragma once

#define _WINSOCKAPI_

#include "interfaces\INetworkDataObserver.h"

#include <windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <thread>
#include <atomic>
#include <algorithm>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

class NetworkManager {
public:
    NetworkManager();
    ~NetworkManager();

    void SendData(const std::string& data, const std::string& nameReceiver);
    void BroadcastData(const std::string& data); // send to all connected receivers
    void StopReceivingData();

    void StartAcceptConnections(int port);
    void StopAcceptConnections();

    // client mode: connect to a server and start receiving
    bool ConnectToServer(const std::string& serverIp, int port, const std::string& name);

    void AddObserver(INetworkDataObserver* observer);
    void RemoveObserver(INetworkDataObserver* observer);

private:
    void AcceptConnections();
    void ReceiveData(SOCKET clientSocket);

    std::vector<INetworkDataObserver*> observers;
    std::unordered_map<std::string, SOCKET> receivers;

    SOCKET serverSocket;
    std::thread acceptThread;
    std::vector<std::thread> receiveThreads;
    std::atomic<bool> isRunningRecieve;
    std::atomic<bool> isRunningAccept;
};
