# Mate Assistant (Terminal Mode)

## Overview

Mate Assistant is a lightweight client-daemon application written in C, designed to communicate with the **Ollama AI API** through a local daemon. The project demonstrates a proof-of-concept workflow where:

- The **client** sends requests (messages) to the **daemon** via a **Unix socket**.
- The **daemon** forwards these requests to the **Ollama AI API**.
- The **daemon** receives the AI-generated response and sends it back to the **client**.
- Both the client and daemon run in separate terminal windows for simplicity.

This setup allows users to experiment with AI interactions locally without building a full service architecture.

---

## Installation

### Prerequisites

Make sure you have the following installed:

- `gcc` or any C compiler
- `make` (optional but recommended)
- `curl`
- Ollama API running locally (`localhost:11434`)

### Build

Clone the repository and navigate to the project folder:

```bash
    git clone https://github.com/MelekBenamar/Mate.git
    cd mate-assistant
```

Build the daemon and client:

```bash

mkdir -p build

# Build daemon
gcc -Wall -Wextra -std=c99 -o build/mate-daemon src/daemon/mate_daemon.c src/common/utils.c

# Build client
gcc -Wall -Wextra -std=c99 -o build/mate-client src/client/mate_client.c
```

## Usage

### Running the Daemon

Open a terminal and start the daemon:

```bash
./build/mate-daemon
```

### Running the Client

Open another terminal to send a message:

```bash
./build/mate-client "Hello Mate, how are you?"
```

The client will connect to the daemon via the socket, send the message, and display the response returned by the AI.