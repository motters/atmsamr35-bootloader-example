#include <GlobalStartup.h>

/**
 * Application
 */
int main()
{
    configASF();

    printf("Application loaded V0.0.1 \r\n");

    printf("Trigger using a break point on me! \r\n");

    while(true);
}

