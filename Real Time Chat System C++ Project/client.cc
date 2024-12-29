#include <iostream>
#include <thread>
#include <cstring>
#include <unistd.h>
#include "client.h"

const char *SERVER_IP = "127.0.0.1";

void receiveMessages(int socket) {
    char buffer[1024] = {0};
    while (true) {
        int bytesRead = read(socket, buffer, 1024);
        if (bytesRead <= 0) break;  // Connection closed

        // Clear the current input line and display the received message
        std::cout << "\r\033[K";  // Clear the current line
        std::cout << buffer << std::endl;

        // Redisplay the input prompt after the message
        std::cout << "Enter recipient (/quit to exit): ";
        std::cout.flush();  // Ensure the prompt appears correctly
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

    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    send(clientSocket, username.c_str(), username.length(), 0);  // Send username to server

    std::thread recvThread(receiveMessages, clientSocket);

    std::string recipient, message;
    while (true) {
        std::cout << "Enter recipient (/quit to exit): ";
        std::getline(std::cin, recipient);
        if (recipient == "/quit") break;

        std::cout << "Enter your message: ";
        std::getline(std::cin, message);

        std::string formattedMessage = recipient + ":" + message;  // Format: recipient:message
        send(clientSocket, formattedMessage.c_str(), formattedMessage.length(), 0);
    }

    close(clientSocket);
    recvThread.join();
    return 0;
}
