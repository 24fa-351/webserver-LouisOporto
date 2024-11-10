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

#define PORT 80 // Port default
#define LISTEN_BACKLOG 5


// Usage: ./webserver <port>

int respond_to_http_client_message(int socket_fd, http_client_message_t* http_msg) {
  char* response = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
  write(socket_fd, response, strlen(response));
  return 0;
}

void handleConnection(int* sockFdPtr) {
    int clientSocket = *sockFdPtr;
    free(sockFdPtr);

    while(1) {
        printf("Handling connection on %d\n", clientSocket);
        http_client_message_t* http_msg;
        http_read_reuslt_t result;

        read_http_client_message(clientSocket, &http_msg, &result);
        if(result == BAD_REQUEST) {
            printf("Bad request\n");
            close(clientSocket);
            return;
        } else if(result == CLOSED_CONNECTION) {
            printf("Closed connection\n");
            close(clientSocket);
            return;
        }
        
        respond_to_http_client_message(clientSocket, http_msg);
        http_client_message_free(http_msg);
    }
    printf("Done with connection %d\n", clientSocket);
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