#include <stdio.h>

int main(void)
{
    int cnt=0;

    for (cnt=0; cnt<0xffffffff; cnt++) {
        system("/sbin/modprobe bcmdhd");
        sleep(4);

        system("/sbin/modprobe bcmdhd");
        sleep(4);

        printf("==================================== %d ====================================\n", cnt);
        system("/usr/bin/free -b && date");
        printf("\n");
        sleep(2);
    }

    return 0;
}

