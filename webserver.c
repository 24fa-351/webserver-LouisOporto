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

void msg_to_expressions(char* msg, int* a, int* b, char* op) {
    sscanf(msg, "%d %c %d", a, op, b);
}

void calc_to_client(int socket_fd, char* expression) {
    int a;
    int b;
    char op;

    msg_to_expressions(expression, &a, &b, &op);

    int result;
    if(op == '+') {
        result = a + b;
    }
    if(op == '-') {
        result = a - b;
    }
    if(op == '*') {
        result = a * b;
    }
    if(op == '/') {
        result = a / b;
    }

    char response[1024];
    sprintf(response, "Result: %d\n", result);
    write(socket_fd, response, strlen(response));
}

void static_to_client(int socket_fd, char* path) {
    char response[1024];
    char* file_path = path + 7;

    FILE* file = fopen(file_path, "r");
    if(file == NULL) {
        sprintf(response, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
        write(socket_fd, response, strlen(response));
        return;
    }

    sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length: ");
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char file_content[file_size];
    fread(file_content, 1, file_size, file);
    fclose(file);

    write(socket_fd, response, strlen(response));
    write(socket_fd, file_content, file_size);
}

// Prints a proper html that lists number of requests (total recevied bytes and sent bytes)
void get_stats_to_client(int socket_fd) {
    char response[1024];

    sprintf(response, "Stats: \n");

    write(socket_fd, response, strlen(response));
}
int respond_to_http_client_message(int socket_fd, http_client_message_t* http_msg) {
    
    if(strncmp(http_msg->method, "GET ", 4) == 0) {
        char* response = "HTTP/1.1 Good Request\r\nContent-Length: 0\r\n\r\n";
        write(socket_fd, response, strlen(response));
        return 0;
    }
    if(strncmp(http_msg->method, "/static", 7) == 0) {
        // Implement /static
        static_to_client(socket_fd, http_msg->method);
        return 0;
    }
    if(strncmp(http_msg->method, "/stats", 6) == 0) {
        // Implement /stats
        get_stats_to_client(socket_fd);
        return 0;
    }
    if(strncmp(http_msg->method, "/calc", 5) == 0) {
        // Implement /calc
        calc_to_client(socket_fd, http_msg->method + 6);
        return 0;
    }
}

void handleConnection(int* sockFdPtr) {
    int clientSocket = *sockFdPtr;
    free(sockFdPtr);

        printf("Handling connection on %d\n", clientSocket);
    while(1) {
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

    printf("Server started on port: %d\n", port);

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