#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

typedef struct msg {
    char* method;
    char* path;
    char* http_version;
    char* body;
    int body_length;
    char* headers;
} http_client_message_t;

typedef enum { BAD_REQUEST, CLOSED_CONNECTION, MESSAGE } http_read_reuslt_t;

// reponse: a message
void read_http_client_message(int socket_fd, http_client_message_t** http_msg,
                              http_read_reuslt_t* result);

void http_client_message_free(http_client_message_t* http_msg);

#endif