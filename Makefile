CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE
BUILDDIR = build
SRCDIR = src

DAEMON_SOURCES = $(SRCDIR)/daemon/mate_daemon.c
CLIENT_SOURCES = $(SRCDIR)/client/mate_client.c

all: $(BUILDDIR)/mate-daemon $(BUILDDIR)/mate

$(BUILDDIR)/mate-daemon: $(DAEMON_SOURCES)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILDDIR)/mate: $(CLIENT_SOURCES)
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^

install: all
	sudo cp $(BUILDDIR)/mate-daemon /usr/local/bin/
	sudo cp $(BUILDDIR)/mate /usr/local/bin/
	sudo cp config/mate.service /etc/systemd/system/
	sudo systemctl daemon-reload

clean:
	rm -rf $(BUILDDIR)

.PHONY: all install clean