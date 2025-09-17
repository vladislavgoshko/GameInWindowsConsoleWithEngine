#include "network\NetworkManager.h"
#include <sstream>
NetworkManager::NetworkManager() : serverSocket(INVALID_SOCKET), 
isRunningRecieve(false), isRunningAccept(false) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

NetworkManager::~NetworkManager() {
    StopAcceptConnections();
    StopReceivingData();
    WSACleanup();
}

void NetworkManager::StartAcceptConnections(int port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create server socket!\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind server socket!\n";
        closesocket(serverSocket);
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on server socket!\n";
        closesocket(serverSocket);
        return;
    }

    isRunningAccept = true;
    isRunningRecieve = true;
    acceptThread = std::thread(&NetworkManager::AcceptConnections, this);
    std::cout << "Accepting connections on port " << port << "...\n";
}

void NetworkManager::AcceptConnections() {
    while (isRunningAccept) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept connection!\n";
            continue;
        }

        char clientName[256] = { 0 };
        int n = recv(clientSocket, clientName, sizeof(clientName) - 1, 0);
        if (n > 0) {
            clientName[n] = '\0';
            receivers[clientName] = clientSocket;
            std::cout << "Client connected: " << clientName << "\n";
        } else {
            // no client name sent, generate a placeholder
            std::string gen = "client_" + std::to_string((int)clientSocket);
            receivers[gen] = clientSocket;
            std::cout << "Client connected (no name): " << gen << "\n";
        }

        receiveThreads.emplace_back(&NetworkManager::ReceiveData, this, clientSocket);
    }
}

void NetworkManager::ReceiveData(SOCKET clientSocket) {
    char buffer[1024];
    std::stringstream ss;

    ss << "[ReceiveData] Started for client socket: " << (int)clientSocket << "\n";
    OutputDebugStringA(ss.str().c_str());

    while (isRunningRecieve) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            ss.str("");
            ss << "[ReceiveData] Connection closed or error on socket: "
                << (int)clientSocket << ", bytesReceived=" << bytesReceived << "\n";
            OutputDebugStringA(ss.str().c_str());

            closesocket(clientSocket);
            return;
        }

        buffer[bytesReceived] = '\0';
        std::string message(buffer);

        ss.str("");
        ss << "[ReceiveData] Received " << bytesReceived << " bytes from socket "
            << (int)clientSocket << ": " << message << "\n";
        OutputDebugStringA(ss.str().c_str());

        for (auto observer : observers) {
            observer->HandleNetworkData(message);
        }

        // ≈сли это сервер, пересылаем всем другим клиентам
        if (serverSocket != INVALID_SOCKET) {
            for (auto& kv : receivers) {
                if (kv.second != clientSocket) {
                    send(kv.second, message.c_str(), (int)message.length(), 0);
                }
            }
        }
    }

    ss.str("");
    ss << "[ReceiveData] Stopped for client socket: " << (int)clientSocket << "\n";
    OutputDebugStringA(ss.str().c_str());
}


void NetworkManager::StopAcceptConnections() {
    isRunningAccept = false;
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }
    if (acceptThread.joinable()) {
        acceptThread.join();
    }
}

void NetworkManager::SendData(const std::string& data, const std::string& nameReceiver) {
    auto it = receivers.find(nameReceiver);
    if (it != receivers.end()) {
        send(it->second, data.c_str(), (int)data.length(), 0);
    }
    else {
        std::cerr << "Receiver " << nameReceiver << " not found!\n";
    }
}

void NetworkManager::BroadcastData(const std::string& data) {
    for (auto& kv : receivers) {
        send(kv.second, data.c_str(), (int)data.length(), 0);
    }
}

void NetworkManager::StopReceivingData() {
    isRunningRecieve = false;
    for (auto& th : receiveThreads) {
        if (th.joinable()) th.join();
    }
}

bool NetworkManager::ConnectToServer(const std::string& serverIp, int port, const std::string& name) {
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) return false;
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, serverIp.c_str(), &addr.sin_addr);
    if (connect(s, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(s);
        return false;
    }
	receivers["server"] = s;
    // send name
    send(s, name.c_str(), (int)name.length(), 0);
    // start receive thread for server messages
    isRunningRecieve = true;
    receiveThreads.emplace_back(&NetworkManager::ReceiveData, this, s);
    return true;
}

void NetworkManager::AddObserver(INetworkDataObserver* observer) {
    observers.push_back(observer);
}

void NetworkManager::RemoveObserver(INetworkDataObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}
