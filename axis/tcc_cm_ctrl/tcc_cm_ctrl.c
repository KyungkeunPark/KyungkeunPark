
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILENAME "/dev/tcc_cm_ctrl"

#define IOCTL_GET_GEAR_STATUS 5

int main(void)
{
    int dev, ret;
    int cmd = 3;
    int gear_status;

    dev = open(DEVICE_FILENAME, O_RDWR|O_NDELAY);
    if (dev < 0) {
        printf("%s Fiel open fail 0x%\n", DEVICE_FILENAME, dev);
        return -1;
    }

    ret = ioctl(dev, IOCTL_GET_GEAR_STATUS, (unsigned long *)&gear_status);
    printf("status:0x%x ret:0x%x\n", gear_status, ret);

    ret = close(dev);

    return 0;
}

