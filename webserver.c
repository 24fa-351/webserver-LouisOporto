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

#define PORT 25565
#define LISTEN_BACKLOG 5

int respond_to_http_client_message(int socket_fd, https_client_message_t* http_msg) {
  char* response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
  write(socket_fd, response, strlen(response));
  return 0;
}

// innput pointer is freed by this funtion
void handleConnection(int* sock_fd_ptr) {   
    int sock_fd = *sock_fd_ptr;
    free(sock_fd_ptr);

    printf("Handling connection %d\n", sock_fd);
    char buffer[1024];
    int bytes_read = read(sock_fd, buffer, sizeof(buffer));
    printf("Received: %s\n", buffer);
    write(sock_fd, buffer, bytes_read);

    printf("Done with handling connection %d\n", sock_fd);
}
int main(int argc, char* argv[]) {
    
}