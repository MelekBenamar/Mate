#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

int query_ollama(const char* prompt, char* response, size_t response_size);

#endif // UTILS_H
