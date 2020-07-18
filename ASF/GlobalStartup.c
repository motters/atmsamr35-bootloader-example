#include "GlobalStartup.h"

#include <asf.h>


void configASF()
{
    system_init();

    sio2host_init();

    delay_init();

    delay_ms(100);
}
