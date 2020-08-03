#include "print_array.h"

#include <sio2host.h>

void print(const char *data, uint8_t length)
{
    sio2host_tx((uint8_t*)data, length);
}
