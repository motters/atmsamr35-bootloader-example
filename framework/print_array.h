#pragma once

#include <asf.h>

// Easy to use macro
#define PRINT(msg) \
    print(msg, sizeof(msg) -1);

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Use of printf to reduce flash usage
 *
 * @param data
 * @param length
 */
void print(const char *data, uint8_t length);


#ifdef __cplusplus
}
#endif