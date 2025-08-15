#define _POSIX_C_SOURCE 200809L  // For popen/pclose
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "utils.h"

int query_ollama(const char* prompt, char* response, size_t response_size) {
    FILE* fp;
    char command[MAX_MESSAGE_SIZE + 256];

    // Escape quotes in prompt
    char escaped_prompt[MAX_MESSAGE_SIZE * 2];
    size_t j = 0;
    for (size_t i = 0; i < strlen(prompt) && j < sizeof(escaped_prompt) - 2; i++) {
        if (prompt[i] == '"') {
            escaped_prompt[j++] = '\\';
            escaped_prompt[j++] = '"';
        } else {
            escaped_prompt[j++] = prompt[i];
        }
    }
    escaped_prompt[j] = '\0';

    snprintf(command, sizeof(command),
        "curl -s -X POST http://localhost:11434/api/generate "
        "-H 'Content-Type: application/json' "
        "-d '{\"model\": \"llama3\", \"prompt\": \"%s\", \"stream\": false}'",
        escaped_prompt);

    fp = popen(command, "r");  // popen returns FILE*, so now compiler knows it
    if (!fp) {
        snprintf(response, response_size, "Error: could not open Ollama");
        return -1;
    }

    char json_output[MAX_RESPONSE_SIZE] = {0};
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), fp)) {
        strncat(json_output, buffer, sizeof(json_output) - strlen(json_output) - 1);
    }
    pclose(fp);

    // Extract the "response" value
    char* start = strstr(json_output, "\"response\":\"");
    if (start) {
        start += strlen("\"response\":\"");
        char* end = strchr(start, '"');
        if (end) {
            size_t len = end - start;
            if (len >= response_size) len = response_size - 1;
            strncpy(response, start, len);
            response[len] = '\0';
        } else {
            snprintf(response, response_size, "Error: could not parse response");
        }
    } else {
        snprintf(response, response_size, "Error: no response field found");
    }

    printf("Ollama extracted response: %s\n", response);
    return 0;
}
