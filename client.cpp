#include <iostream>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

// Function to handle receiving messages in a background thread
void receiveMessages(int serverSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "\n[System] Connection lost from server." << std::endl;
            break;
        }
        std::cout << "\n[Server]: " << buffer << std::endl;
        std::cout << "[You]: " << std::flush;
    }
}

int main() {
    // 1. Create a TCP socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // 2. Configure target connection details (Localhost)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    // 3. Connect to the Server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed. Make sure server.cpp is running first." << std::endl;
        return 1;
    }
    std::cout << "Connected to the server!" << std::endl;

    // 4. Spin up a separate thread to handle simultaneous receiving (Full Duplex)
    std::thread receiveThread(receiveMessages, clientSocket);
    receiveThread.detach();

    // 5. Main thread handles sending messages
    std::string message;
    while (true) {
        std::cout << "[You]: " << std::flush;
        std::getline(std::cin, message);
        if (message == "exit") break;

        send(clientSocket, message.c_str(), message.length(), 0);
    }

    // 6. Cleanup
    close(clientSocket);
    return 0;
}

