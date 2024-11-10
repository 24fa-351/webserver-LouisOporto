#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "http_message.h"

#define PORT 25565 // Port default
#define LISTEN_BACKLOG 5

// Usage: ./webserver <port>

int respond_to_http_client_message(int socket_fd, https_client_message_t* http_msg) {
  char* response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
  write(socket_fd, response, strlen(response));
  return 0;
}

void broadcastMessage(char* message, int clientSocket) {
    send(clientSocket, message, strlen(message), 0);
}

void handleConnection(int* sockFdPtr) {
    int clientSocket = *sockFdPtr;
    free(sockFdPtr);
    char message[1024];
    int readBytes;

    // Receive data from the client
    while ((readBytes = recv(clientSocket, message, 1024, 0)) > 0) {
        message[readBytes] = '\0';
        printf("Client: %s\n", message);

        // Broadcast message back to user
        broadcastMessage(message, clientSocket);
    }

    // Close client socket
    close(clientSocket);
    pthread_exit(NULL);
}

void startServer(int port) {
    int serverSocket;
    int clientSocket;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;
    socklen_t addressSize = sizeof(clientAddress);

    // Initiate listening port
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(serverSocket < 0) {
        perror("Error creating listening port");
        exit(1);
    }

    // Configure server socket
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind server socket to server address
    if(bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding server address to server socket");
        exit(1);
    }

    // Limit the number of client to 5
    if(listen(serverSocket, LISTEN_BACKLOG) < 0) {
        perror("Error listening for incoming connections");
        exit(1);
    }

    printf("Serve started on port: %d\n", port);

    while(1) {
        pthread_t thread;
        int *clientSocketPtr = (int*)malloc(sizeof(int));

        *clientSocketPtr = accept(serverSocket, (struct sockaddr*)&clientAddress, &addressSize);

        if(*clientSocketPtr < 0) {
            perror("Error accepting client connection");
            exit(1);
        } else {
            printf("Accepted connection on %d\n", *clientSocketPtr);
        }

        pthread_create(&thread, NULL, (void*(*)(void*))handleConnection, (void*)clientSocketPtr);
    }
}

int main(int argc, char* argv[]) {
    int port = PORT;

    if(argc > 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    } else if(argc == 2) {
        sscanf(argv[1], "%d", &port);
    } else {
        printf("Using default port 25565\n");
    }

    printf("Port: %d\n", port);
    startServer(port);

    return 0;
}