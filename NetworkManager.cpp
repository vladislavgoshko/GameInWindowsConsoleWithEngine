#include "NetworkManager.h"

NetworkManager::NetworkManager() : serverSocket(INVALID_SOCKET), 
isRunningRecieve(false), isRunningAccept(false) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

NetworkManager::~NetworkManager() {
    StopAcceptConnections();
    WSACleanup();
}

void NetworkManager::StartAcceptConnections(int port) {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Ошибка создания сокета!\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Ошибка привязки сокета!\n";
        closesocket(serverSocket);
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Ошибка при вызове listen!\n";
        closesocket(serverSocket);
        return;
    }

    acceptThread = std::thread(&NetworkManager::AcceptConnections, this);
    isRunningAccept = true;
    isRunningRecieve = true;
    std::cout << "Ожидание подключений на порту " << port << "...\n";
}

void NetworkManager::AcceptConnections() {
    while (isRunningAccept) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Ошибка принятия подключения!\n";
            continue;
        }

        char clientName[256] = { 0 };
        recv(clientSocket, clientName, sizeof(clientName), 0);
        receivers[clientName] = clientSocket;
        std::cout << "Клиент подключился: " << clientName << "\n";

        receiveThreads.emplace_back(&NetworkManager::ReceiveData, this, clientSocket);
    }
}

void NetworkManager::ReceiveData(SOCKET clientSocket) {
    char buffer[1024];
    while (isRunningRecieve) {
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            return;
        }
        buffer[bytesReceived] = '\0';

        std::string message(buffer);
        for (auto observer : observers) {
            observer->HandleNetworkData(message);
        }
    }
}

void NetworkManager::StopAcceptConnections() {
    isRunningAccept = false;
    /*if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }*/
    if (acceptThread.joinable()) {
        acceptThread.join();
    }
    /*for (auto& th : receiveThreads) {
        if (th.joinable()) th.join();
    }
    receivers.clear();*/
}

void NetworkManager::SendData(const std::string& data, const std::string& nameReceiver) {
    auto it = receivers.find(nameReceiver);
    if (it != receivers.end()) {
        send(it->second, data.c_str(), data.length(), 0);
    }
    else {
        std::cerr << "Получатель " << nameReceiver << " не найден!\n";
    }
}

void NetworkManager::StopReceivingData() {
    isRunningRecieve = false;
    for (auto& th : receiveThreads) {
        if (th.joinable()) th.join();
    }
}

void NetworkManager::AddObserver(INetworkDataObserver* observer) {
    observers.push_back(observer);
}

void NetworkManager::RemoveObserver(INetworkDataObserver* observer) {
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}
