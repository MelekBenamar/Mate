#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "../common/protocol.h"

#define SOCKET_PATH "/tmp/mate.sock"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s \"your message\"\n", argv[0]);
        return 1;
    }

    int client_side_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_side_fd < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_side_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(client_side_fd);
        return 1;
    }

    ai_request_t req = {0};
    for (int i = 1; i < argc; i++) {
        strncat(req.message, argv[i], MAX_MESSAGE_SIZE - strlen(req.message) - 1);
        if (i < argc - 1) strncat(req.message, " ", MAX_MESSAGE_SIZE - strlen(req.message) - 1);
    }
    req.message_len = strlen(req.message);

    if (send(client_side_fd, &req, sizeof(req), 0) < 0) {
        perror("send");
        close(client_side_fd);
        return 1;
    }

    ai_response_t res = {0};
    ssize_t n = recv(client_side_fd, &res, sizeof(res), 0);
    if (n > 0) {
        res.response[res.response_len] = '\0';
        printf("Response (%d): %s\n", res.status, res.response);
    } else {
        printf("No response received\n");
    }

    close(client_side_fd);
    return 0;
}
