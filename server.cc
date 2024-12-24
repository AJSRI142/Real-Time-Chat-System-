#include "server.h"

const int PORT = 8080;
const int MAX_CLIENTS = 10;

void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    while (true) {
        int bytesRead = read(clientSocket, buffer, 1024);
        if (bytesRead <= 0) break;  // Client disconnected
        std::cout << "Message received: " << buffer << std::endl;
        send(clientSocket, buffer, bytesRead, 0);  // Echo back
        memset(buffer, 0, sizeof(buffer));
    }
    close(clientSocket);
    std::cout << "Client disconnected.\n";
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
