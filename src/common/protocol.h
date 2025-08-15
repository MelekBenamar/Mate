#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>

#define SOCKET_PATH "/tmp/mate.sock"
#define MAX_MESSAGE_SIZE 4096
#define MAX_RESPONSE_SIZE 8192

typedef struct {
    char message[MAX_MESSAGE_SIZE];
    size_t message_len;  // Changed from int to size_t
} ai_request_t;

typedef struct {
    char response[MAX_RESPONSE_SIZE];
    size_t response_len;  // Changed from int to size_t
    int status; // 0 = success, -1 = error
} ai_response_t;

// Function prototypes
int create_socket_server(void);
int connect_to_daemon(void);
int send_request(int sockfd, const char* message);
int receive_response(int sockfd, char* buffer, size_t buffer_size);

#endif