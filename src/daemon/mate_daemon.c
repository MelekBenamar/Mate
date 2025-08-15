#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <errno.h>

#include "../common/utils.h"
#include "../common/protocol.h"

#define SOCKET_PATH "/tmp/mate.sock"

static int server_fd = -1;
static int running = 1;

void handle_signal(int sig) {
    printf("Signal %d received, shutting down...\n", sig);
    running = 0;
}

int create_socket_server() {
    struct sockaddr_un addr;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return -1; }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(server_fd); return -1;
    }

    if (listen(server_fd, 5) < 0) { perror("listen"); close(server_fd); return -1; }

    printf("Daemon: socket server ready at %s\n", SOCKET_PATH);
    return 0;
}

void handle_client(int client_fd) {
    ai_request_t request;
    ai_response_t response;

    ssize_t n = recv(client_fd, &request, sizeof(request), 0);
    if (n <= 0) return;

    request.message[n] = '\0';
    printf("Daemon received: %s\n", request.message);

    if (query_ollama(request.message, response.response, sizeof(response.response)) != 0) {
        snprintf(response.response, sizeof(response.response), "Error querying Ollama");
        response.status = -1;
    } else {
        response.status = 0;
        response.response_len = strlen(response.response);
    }

    send(client_fd, &response, sizeof(response), 0);
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    if (create_socket_server() != 0) exit(1);

    while (running) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) { if (errno == EINTR) continue; perror("accept"); continue; }

        handle_client(client_fd);
        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    printf("Daemon stopped\n");
    return 0;
}
