#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

void handleClient(SOCKET clientSocket, int clientNumber) {
    cout << "Handling client " << clientNumber << endl;
    const char* message = "Hello from server!\n";
    send(clientSocket, message, strlen(message), 0);

    //// Close the client socket
    //closesocket(clientSocket);
    //cout << "Client " << clientNumber << " disconnected" << endl;
}

void listenOnPort() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cerr << "WSAStartup failed: " << iResult << endl;
        return;
    }

    // Create a socket
    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return;
    }

    // Bind the socket
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(9090);

    iResult = bind(ListenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        cerr << "Bind failed: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // Listen for incoming connections
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        cerr << "Listen failed: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    std::cout << "Listening on port 9090..." << std::endl;

    // Vector to hold client threads
    vector<thread> clientThreads;
    int clientNumber = 0;

    // Accept and handle incoming connections
    while (true) {
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            cerr << "Accept failed: " << WSAGetLastError() << endl;
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        // Increment and log the client number
        clientNumber++;
        cout << "Accepted connection from client " << clientNumber << endl;

        // Create a new thread for each client connection
        clientThreads.emplace_back(handleClient, ClientSocket, clientNumber);
    }

    // Cleanup
    for (auto& th : clientThreads) {
        if (th.joinable()) {
            th.join();
        }
    }

    closesocket(ListenSocket);
    WSACleanup();
}

int main() {
    // Create and start the listening thread
    thread listeningThread(listenOnPort);

    // Wait for the listening thread to finish
    listeningThread.join();

    return 0;
}
