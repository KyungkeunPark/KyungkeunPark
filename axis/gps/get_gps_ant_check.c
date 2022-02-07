
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define FILENAME "/sys/devices/tcc_gps_dev/ant_open"

int main(void)
{
    int fd;
    int cnt=0, value=0;
    unsigned char buf[6];
    unsigned char data;

    while(1) {
        fd = open(FILENAME, O_RDWR);

        read(fd, &data, sizeof(data));

        printf("0x%x\n", data);

        usleep(250000);

        close(fd);
    }

    return 0;
}

