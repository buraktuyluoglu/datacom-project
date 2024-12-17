#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <time.h>

#define PORT 9999
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

// Array of quotes
const char* quotes[] = {
    "The only way to do great work is to love what you do. - Steve Jobs",
    "Innovation distinguishes between a leader and a follower. - Steve Jobs",
    "Stay hungry, stay foolish. - Steve Jobs",
    "The future belongs to those who believe in the beauty of their dreams. - Eleanor Roosevelt",
    "Success is not final, failure is not fatal. - Winston Churchill"
};

// Array of inspirational messages
const char* inspirational[] = {
    "You are capable of amazing things!",
    "Every day is a new beginning.",
    "Believe you can and you're halfway there.",
    "Make today amazing!",
    "Your potential is endless."
};

#define NUM_QUOTES (sizeof(quotes) / sizeof(quotes[0]))
#define NUM_INSPIRE (sizeof(inspirational) / sizeof(inspirational[0]))

void *handle_client(void *socket_desc) {
    int client_sock = *(int*)socket_desc;
    free(socket_desc);
    char buffer[BUFFER_SIZE] = {0};
    
    // Initialize random seed for each client thread
    srand(time(NULL) ^ (pthread_self()));
    
    while(1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(client_sock, buffer, BUFFER_SIZE, 0);
        
        if(valread <= 0) {
            printf("Client disconnected\n");
            break;
        }
        
        buffer[valread] = '\0';
        
        // Remove newline if present
        char *newline = strchr(buffer, '\n');
        if(newline) *newline = '\0';
        
        const char* response = NULL;
        
        if(strcmp(buffer, "/quote") == 0) {
            int quote_index = rand() % NUM_QUOTES;
            response = quotes[quote_index];
        }
        else if(strcmp(buffer, "/inspire") == 0) {
            int inspire_index = rand() % NUM_INSPIRE;
            response = inspirational[inspire_index];
        }
        else if(strcmp(buffer, "/exit") == 0) {
            send(client_sock, "Goodbye!\n", 9, 0);
            break;
        }
        else {
            response = "Unknown command. Available commands: /quote, /inspire, /exit\n";
        }
        
        send(client_sock, response, strlen(response), 0);
    }
    
    closesocket(client_sock);
    return NULL;
}

int main() {
    int server_fd, *new_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    pthread_t thread_id;
    
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    // Configure address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server is listening on port %d\n", PORT);
    
    while(1) {
        // Accept new connection
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Accept failed");
            continue;
        }
        
        printf("New client connected\n");
        
        // Create new socket pointer for the thread
        new_sock = malloc(sizeof(int));
        *new_sock = new_socket;
        
        // Create new thread for client
        if (pthread_create(&thread_id, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Thread creation failed");
            free(new_sock);
            continue;
        }
        
        // Detach thread
        pthread_detach(thread_id);
    }
    
    // Cleanup Winsock
    WSACleanup();
    
    return 0;
} 