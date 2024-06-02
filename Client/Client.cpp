#include <iostream>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")

void communicateWithServer(SOCKET ConnectSocket) {
    char recvbuf[512];
    int recvbuflen = 512;

    // Receive data from server
    int iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        cout << "Received message from server: " << string(recvbuf, iResult) << endl;
    }
    else if (iResult == 0) {
        cout << "Connection closed" << endl;
    }
    else {
        cerr << "Recv failed: " << WSAGetLastError() <<endl;
    }

    // Cleanup
    closesocket(ConnectSocket);
}

int main() {
    WSADATA wsaData;
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cerr << "WSAStartup failed: " << iResult << endl;
        return 1;
    }

    // Create a socket
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    // Setup the server address structure
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9090);
    inet_pton(AF_INET, "192.168.0.119", &serverAddr.sin_addr);

    // Connect to server
    iResult = connect(ConnectSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        cerr << "Connect failed: " << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server on port 9090" << std::endl;

    // Create a thread for communicating with the server
    thread communicationThread(communicateWithServer, ConnectSocket);

    // Do other stuff in the main thread if necessary

    // Wait for the communication thread to finish
    communicationThread.join();

    // Cleanup Winsock
    WSACleanup();

    return 0;
}
