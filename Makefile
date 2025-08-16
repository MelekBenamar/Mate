# Makefile for Mate Assistant

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
BUILD_DIR = build

DAEMON_SRC = src/daemon/mate_daemon.c src/common/utils.c
CLIENT_SRC = src/client/mate_client.c src/common/utils.c

DAEMON_BIN = $(BUILD_DIR)/mate-daemon
CLIENT_BIN = $(BUILD_DIR)/mate

SERVICE_FILE = config/mate-daemon.service

.PHONY: all clean install uninstall

all: $(DAEMON_BIN) $(CLIENT_BIN)

$(DAEMON_BIN): $(DAEMON_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(DAEMON_SRC)

$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC)

clean:
	rm -rf $(BUILD_DIR)

install: all
	# Copy daemon binary
	sudo mkdir -p /usr/local/bin
	sudo cp $(DAEMON_BIN) /usr/local/bin/
	# Copy client binary
	sudo cp $(CLIENT_BIN) /usr/local/bin/
	# Copy systemd service file
	sudo cp $(SERVICE_FILE) /etc/systemd/system/
	# Reload systemd and enable service
	sudo systemctl daemon-reload
	sudo systemctl enable mate-daemon.service
	sudo systemctl start mate-daemon.service
	@echo "Installation complete. Daemon running as a system service."

uninstall:
	# Stop and disable service
	sudo systemctl stop mate-daemon.service
	sudo systemctl disable mate-daemon.service
	# Remove binaries
	sudo rm -f /usr/local/bin/mate-daemon
	sudo rm -f /usr/local/bin/mate-client
	# Remove service file
	sudo rm -f /etc/systemd/system/mate-daemon.service
	sudo systemctl daemon-reload
	@echo "Mate Assistant uninstalled."
