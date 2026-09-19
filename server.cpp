#include <iostream>
#include <cstring>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Function to handle receiving messages in a background thread
void receiveMessages(int clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "\n[System] Client disconnected." << std::endl;
            break;
        }
        std::cout << "\n[Client]: " << buffer << std::endl;
        std::cout << "[You]: " << std::flush;
    }
}

int main() {
    // 1. Create a TCP socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // 2. Bind the socket to Localhost (127.0.0.1) and Port 5555
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5555);
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Listens on all local interfaces

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed. Port might be in use." << std::endl;
        return 1;
    }

    // 3. Listen for connections
    listen(serverSocket, 1);
    std::cout << "Server listening on port 5555... Waiting for Client." << std::endl;

    // 4. Accept a client connection
    sockaddr_in clientAddr;
    socklen_t clientSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientSize);
    if (clientSocket < 0) {
        std::cerr << "Failed to accept connection." << std::endl;
        return 1;
    }
    std::cout << "Connected to client successfully!" << std::endl;

    // 5. Spin up a separate thread to handle simultaneous receiving (Full Duplex)
    std::thread receiveThread(receiveMessages, clientSocket);
    receiveThread.detach(); // Allow it to run independently

    // 6. Main thread handles sending messages
    std::string message;
    while (true) {
        std::cout << "[You]: " << std::flush;
        std::getline(std::cin, message);
        if (message == "exit") break;
        
        send(clientSocket, message.c_str(), message.length(), 0);
    }

    // 7. Cleanup
    close(clientSocket);
    close(serverSocket);
    return 0;
}

