#pragma once

#include <asf.h>

// Easy to use macro
#define PRINT(msg) \
    print(msg, sizeof(msg) -1);

/**
 * Use of printf to reduce flash usage
 *
 * @param data
 * @param length
 */
void print(const char *data, uint8_t length);
