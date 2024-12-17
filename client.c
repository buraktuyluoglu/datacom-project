#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 9999
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
    
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char message[BUFFER_SIZE] = {0};
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return -1;
    }
    
    // Configure server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 address from text to binary
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        return -1;
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return -1;
    }
    
    printf("Connected to server! Available commands:\n");
    printf("/quote  - Get a random quote\n");
    printf("/inspire - Get an inspirational message\n");
    printf("/exit   - Quit the program\n\n");
    
    while(1) {
        printf("Enter command: ");
        fgets(message, BUFFER_SIZE, stdin);
        
        // Send message to server
        send(sock, message, strlen(message), 0);
        
        // Clear buffer and receive response
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        
        if(valread <= 0) {
            printf("\nServer disconnected\n");
            break;
        }
        
        printf("\nServer: %s\n", buffer);
        
        // Check if client wants to exit
        if(strncmp(message, "/exit", 5) == 0) {
            break;
        }
    }
    
    closesocket(sock);
    WSACleanup();
    return 0;
} 