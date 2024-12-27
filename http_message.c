#include "http_message.h"

#include <stdbool.h>
#include <stdio.h>  // For testing
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
// Handle the case where she needs to implement /static and /stats and /calc
bool is_complete_http_message(char* buffer) {
    if (strlen(buffer) < 2) {
        return false;
    }
    if (strncmp(buffer, "GET ", 4) == 0) {
        if (strncmp(buffer + strlen(buffer) - 4, "\r\n\r\n", 4) != 0) {
            return false;
        }
    }
    return true;
}

void read_http_client_message(int socket_fd, http_client_message_t** msg,
                              http_read_reuslt_t* result) {
    *msg = malloc(sizeof(http_client_message_t));
    char buffer[1024];
    strcpy(buffer, "");

    while (!is_complete_http_message(buffer)) {
        int bytes_read = read(socket_fd, buffer + strlen(buffer),
                              sizeof(buffer) - strlen(buffer));
        if (bytes_read == 0) {
            *result = CLOSED_CONNECTION;
            return;
        }
        if (bytes_read < 0) {
            *result = BAD_REQUEST;
            return;
        }
    }

    (*msg)->method = strdup(buffer);
    *result = MESSAGE;
}

void http_client_message_free(http_client_message_t* msg) { free(msg); }
