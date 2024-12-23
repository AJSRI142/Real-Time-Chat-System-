#include "client.h"

void receive_messages(SOCKET socket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cout << "Server disconnected\n";
            break;
        }

        std::cout << "Server: " << buffer << "\n";
    }

    #ifdef _WIN32
    closesocket(socket);
    #else
    close(socket);
    #endif
}

void initialize_winsock() {
    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        exit(1);
    }
    #endif
}

int main() {
    initialize_winsock();

    SOCKET client_socket;
    struct sockaddr_in server_address;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported\n";
        return 1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }

    std::cout << "Connected to the server\n";

    std::thread receiver_thread(receive_messages, client_socket);
    receiver_thread.detach();

    std::string message;
    while (true) {
        std::getline(std::cin, message);

        if (send(client_socket, message.c_str(), message.size(), 0) < 0) {
            std::cerr << "Send failed\n";
            break;
        }
    }

    #ifdef _WIN32
    WSACleanup();
    #endif

    return 0;
}
