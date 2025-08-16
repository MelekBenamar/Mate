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
- Ollama API running locally (`localhost:11434`) with llama3 model (Importent !!)

### Build

Clone the repository and navigate to the project folder:

```bash
    git clone https://github.com/MelekBenamar/Mate.git
    cd mate-assistant
```

Activate Mate:

```bash
make clean
make install
```

## Usage

Open a terminal and ask mate, thats it:

```bash
mate "<your question here>"
```

## Notes

Please it's important to start Ollama and run llama3 model:

```bash
ollama serve
ollama run llama3
```

You can uninstall Mate any time by just doing:

```bash
make clean
make uninstall
```

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Usage Restrictions
While this project is open-source, any commercial use, distribution, or derivative work should be made in compliance with the terms of the license.
