#include "server.h"
#include <map>
#include <sstream>  // For parsing recipient and message

const int PORT = 8080;
const int MAX_CLIENTS = 10;

std::map<std::string, int> usernameToSocket;  // Map username to client socket

void handleClient(int clientSocket) {
    char buffer[1024] = {0};

    // Receive the username
    int bytesRead = read(clientSocket, buffer, 1024);
    if (bytesRead <= 0) {
        close(clientSocket);
        std::cout << "Client disconnected before sending username.\n";
        return;
    }
    std::string username(buffer);
    usernameToSocket[username] = clientSocket;
    std::cout << username << " has joined the chat.\n";
    memset(buffer, 0, sizeof(buffer));

    // Handle messages from the client
    while (true) {
        bytesRead = read(clientSocket, buffer, 1024);
        if (bytesRead <= 0) break;  // Client disconnected

        // Parse the message (format: recipient:message)
        std::string fullMessage(buffer);
        std::string recipient, message;
        std::istringstream stream(fullMessage);
        std::getline(stream, recipient, ':');
        std::getline(stream, message);

        // Find the recipient's socket and forward the message
        if (usernameToSocket.find(recipient) != usernameToSocket.end()) {
            int recipientSocket = usernameToSocket[recipient];
            std::string formattedMessage = username + ": " + message;
            send(recipientSocket, formattedMessage.c_str(), formattedMessage.length(), 0);
        } else {
            std::string errorMessage = "Error: User " + recipient + " is not connected.\n";
            send(clientSocket, errorMessage.c_str(), errorMessage.length(), 0);
        }

        memset(buffer, 0, sizeof(buffer));
    }

    // Cleanup
    close(clientSocket);
    std::cout << username << " has left the chat.\n";
    usernameToSocket.erase(username);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    std::vector<std::thread> threads;
    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            perror("Client accept failed");
            continue;
        }

        std::cout << "New client connected.\n";
        threads.emplace_back(handleClient, clientSocket);  // Handle client in a new thread
    }

    for (auto &t : threads) {
        if (t.joinable()) t.join();
    }

    close(serverSocket);
    return 0;
}
