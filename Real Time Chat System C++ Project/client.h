#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <thread>
#include <cstring> // For using memset

// Platform-specific includes and definitions
#ifdef _WIN32
    // Windows-specific headers for Winsock
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib") // Link the Winsock library
#else
    // Linux/Unix-specific headers for socket programming
    #include <arpa/inet.h> // For inet_pton
    #include <sys/socket.h> // For socket functions
    #include <netinet/in.h> // For sockaddr_in structure
    #include <unistd.h> // For close() function
#endif

// Define the port for the connection
#define PORT 8080

// Function declarations
void receiveMessages(int socket); // Receives messages from the server
std::string username; 
// Windows-specific initialization for Winsock
void initialize_winsock();

#endif // CLIENT_H
