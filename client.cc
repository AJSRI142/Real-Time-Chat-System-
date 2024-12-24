#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const char *SERVER_IP = "127.0.0.1";  // Localhost
const int PORT = 8080;

void receiveMessages(int socket) {
    char buffer[1024] = {0};
    while (true) {
        int bytesRead = read(socket, buffer, 1024);
        std::cout<<(bytesRead)<<std::endl;
        if (bytesRead <= 0) break;  // Connection closed
        std::cout << "Server: " << buffer << std::endl;
        memset(buffer, 0, sizeof(buffer));
    }
    close(socket);
    std::cout << "Disconnected from server.\n";
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        perror("Invalid address");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Connected to server.\n";

    std::thread recvThread(receiveMessages, clientSocket);
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        if (message == "/quit") break;
        send(clientSocket, message.c_str(), message.length(), 0);
    }

    close(clientSocket);
    recvThread.join();
    return 0;
}
