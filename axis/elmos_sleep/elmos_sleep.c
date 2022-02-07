
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILENAME "/dev/el52241"

#define IOCTL_ELMOS_SLEEP   0x10
#define IOCTL_ELMOS_RESUME  0x11

int main(void)
{
	int dev, ret;
	
	printf("Applicatoin : open\n");
	dev = open(DEVICE_FILENAME, O_RDWR|O_NDELAY);
	if (dev < 0) {
		printf("Elmos Fiel open fail 0x%\n", dev);
		return -1;
	}

	printf("Application : Ioctl\n");
	ret = ioctl(dev, IOCTL_ELMOS_SLEEP, 10);
		
	printf("Application : close\n");
	ret = close(dev);

	return 0;
}

