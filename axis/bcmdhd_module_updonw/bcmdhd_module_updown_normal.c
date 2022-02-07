#include <stdio.h>

int main(void)
{
    int cnt=0;

    for (cnt=0; cnt<0xFFFFFFFF; cnt++) {
        printf("========================== %d ==========================\n", cnt);
        system("/usr/bin/free -b && date");
        printf("========================================================\n");
        sleep(5);
    }

    return 0;
}

