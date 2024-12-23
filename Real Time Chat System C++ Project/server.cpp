#include "server.h"

std::vector<SOCKET> clients;
std::mutex clients_mutex;

void handle_client(SOCKET client_socket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cout << "Client disconnected\n";

            #ifdef _WIN32
                closesocket(client_socket);
            #else
                close(client_socket);
            #endif

            clients_mutex.lock();
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            clients_mutex.unlock();
            break;
        }

        std::cout << "Received: " << buffer << "\n";

        clients_mutex.lock();
        for (SOCKET sock : clients) {
            if (sock != client_socket) {
                send(sock, buffer, bytes_received, 0);
            }
        }
        clients_mutex.unlock();
    }
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

    SOCKET server_socket;
    struct sockaddr_in server_address;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    if (listen(server_socket, 5) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "\n";

    while (true) {
        SOCKET client_socket;
        struct sockaddr_in client_address;
        int client_len = sizeof(client_address);

        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed\n";
            continue;
        }

        clients_mutex.lock();
        clients.push_back(client_socket);
        clients_mutex.unlock();

        std::cout << "New client connected\n";

        std::thread(handle_client, client_socket).detach();
    }

    #ifdef _WIN32
    WSACleanup();
    #endif

    return 0;
}
