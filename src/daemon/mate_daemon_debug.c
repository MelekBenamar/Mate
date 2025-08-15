#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>

#include "../common/protocol.h"

static int server_fd = -1;
static volatile sig_atomic_t running = 1;

// Redirect stdout and stderr to syslog
void redirect_output_to_syslog() {
    int log_fd = open("/dev/null", O_WRONLY); // prevent terminal prints
    if (log_fd != -1) {
        dup2(log_fd, STDOUT_FILENO);
        dup2(log_fd, STDERR_FILENO);
        close(log_fd);
    }
}

// Signal handler that logs every signal
void signal_handler(int sig) {
    syslog(LOG_INFO, "DEBUG: Signal received: %d (%s)", sig, strsignal(sig));
    // Comment out the next line if you want to ignore SIGTERM for testing
    running = 0;  
}

int create_socket_server(void) {
    struct sockaddr_un addr;
    
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        syslog(LOG_ERR, "socket() failed: %s", strerror(errno));
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    
    unlink(SOCKET_PATH); // Remove existing socket
    
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        syslog(LOG_ERR, "bind() failed: %s", strerror(errno));
        return -1;
    }
    
    if (listen(server_fd, 5) == -1) {
        syslog(LOG_ERR, "listen() failed: %s", strerror(errno));
        return -1;
    }
    
    return 0;
}

// Minimal client handler for testing
void handle_client(int client_fd) {
    char buffer[256];
    ssize_t n = recv(client_fd, buffer, sizeof(buffer)-1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        syslog(LOG_INFO, "Received message: %s", buffer);
        send(client_fd, "ACK", 3, 0);
    }
}

int main() {
    openlog("mate-daemon", LOG_PID | LOG_NDELAY, LOG_DAEMON);
    syslog(LOG_INFO, "Mate daemon starting");

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGHUP, signal_handler);

    redirect_output_to_syslog(); // redirect all stdout/stderr to syslog

    if (create_socket_server() == -1) {
        syslog(LOG_ERR, "Failed to create socket server");
        exit(1);
    }

    syslog(LOG_INFO, "Socket server created, entering main loop");

    while (running) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno == EINTR) continue;
            syslog(LOG_ERR, "accept() failed: %s", strerror(errno));
            continue;
        }
        handle_client(client_fd);
        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    syslog(LOG_INFO, "Mate daemon stopped");
    closelog();

    return 0;
}
