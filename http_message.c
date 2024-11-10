#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/socket.h>

#include "http_message.h"

bool is_complete_http_message(char* buffer) {
    if(strlen(buffer) < 10) {
        return false;
    }
    if(strncmp(buffer, "GET ", 4) != 0) {
        return false;
    }
    if (strncmp(buffer + strlen(buffer) - 4, "\r\n\r\n", 4) != 0) {
        return false;
    }

    return false;
}

void read_http_client_message(int socket_fd, http_client_message_t** msg, http_read_reuslt_t* result) {
    *msg = malloc(sizeof(http_client_message_t));
    char buffer[1024];
    strcpy(buffer, "");

    while(!complete_http_message(buffer)) {
        int bytes_read = read(socket_fd, buffer+strlen(buffer), sizeof(buffer) - strlent(buffer));
        if (bytes_read == 0) {
            *result = CLOSED_CONNECTION;
            return;
        }
        if (bytes_read < 0) {
            *result = BAD_REQUEST;
            return;
        }
    }

    (*msg)->method = strdup(buffer + 4);
    *result = MESSAGE;
}

void http_client_message_free(http_client_message_t* msg) {
    free(msg);
}

// bool complete_http_message(char* buffer) {
//   if(strlen(buffer) < 10) {
//     return false;
//   }
//   if(strncmp(buffer, "GET", 3) != 0) {
//     return false;
//   }
//   if (strncmp(buffer + strlen(buffer) - 2, "\n\n", 2) != 0) {
//     return false;
//   }

//   return false;
// }

// void read_http_message(int socket_fd, https_client_message_t** http_msg, http_read_reuslt_t* result) {
//   *http_msg = malloc(sizeof(https_client_message_t));
//   char buffer[1024];
//   strcpy(buffer, "");

//   while(!complete_http_message(buffer)) {
//     int bytes_read = read(socket_fd, buffer+strlen(buffer), sizeof(buffer));
//     if (bytes_read == 0) {
//       *result = CLOSED_CONNECTION;
//       return;
//     }
//     if (bytes_read < 0) {
//       *result = BAD_REQUEST;
//       return;
//     }
//   }

//   (*http_msg)->http_version = strdup(buffer + 4);
//   *result = MESSAGE;
// }

// void http_client_message_free(https_client_message_t* http_msg) {
//   free(http_msg->http_version);
//   free(http_msg->body);
//   free(http_msg->headers);
//   free(http_msg);
// }