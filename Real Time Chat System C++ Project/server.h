#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <cstring> // For memset
#include <algorithm> // For std::remove

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#define PORT 8080

extern std::vector<SOCKET> clients;
extern std::mutex clients_mutex;

void handle_client(SOCKET client_socket);

void initialize_winsock();

#endif // SERVER_H
