#define _POSIX_C_SOURCE 200809L  // For popen/pclose
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "utils.h"

int query_ollama(const char* prompt, char* response, size_t response_size) {
    FILE* fp;
    char command[MAX_MESSAGE_SIZE + 4096];

    // Escape quotes in prompt
    char escaped_prompt[MAX_MESSAGE_SIZE];
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


static int hex_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

void decode_json_escapes(char *str) {
    char *src = str, *dst = str;
    while (*src) {
        if (*src == '\\') {
            src++;
            switch (*src) {
                case 'n': *dst++ = '\n'; src++; break;
                case 't': *dst++ = '\t'; src++; break;
                case 'r': *dst++ = '\r'; src++; break;
                case '"': *dst++ = '"';  src++; break;
                case '\\': *dst++ = '\\'; src++; break;
                case 'u': {
                    // Expect 4 hex digits
                    if (src[1] && src[2] && src[3] && src[4]) {
                        int code = (hex_to_int(src[1]) << 12) |
                                   (hex_to_int(src[2]) << 8)  |
                                   (hex_to_int(src[3]) << 4)  |
                                   (hex_to_int(src[4]));
                        src += 5;
                        if (code == 0x003c) { *dst++ = '<'; }
                        else if (code == 0x003e) { *dst++ = '>'; }
                        else if (code == 0x0026) { *dst++ = '&'; }
                        else if (code < 128) { *dst++ = (char)code; }
                        else {
                            // Non-ASCII Unicode (basic UTF-8 encode)
                            if (code <= 0x7FF) {
                                *dst++ = 0xC0 | ((code >> 6) & 0x1F);
                                *dst++ = 0x80 | (code & 0x3F);
                            } else {
                                *dst++ = 0xE0 | ((code >> 12) & 0x0F);
                                *dst++ = 0x80 | ((code >> 6) & 0x3F);
                                *dst++ = 0x80 | (code & 0x3F);
                            }
                        }
                    } else {
                        *dst++ = '\\'; // malformed, keep as-is
                        *dst++ = 'u';
                    }
                    break;
                }
                default:
                    *dst++ = '\\';
                    *dst++ = *src++;
            }
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}