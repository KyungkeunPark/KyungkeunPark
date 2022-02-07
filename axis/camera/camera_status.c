
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILENAME "/dev/video0"

#define BASE_VIDIOC_PRIVATE	192
#define RCAM_GET_STATUS    _IOR  ('V', BASE_VIDIOC_PRIVATE+30, unsigned long)

int main(void)
{
	int fd, ret, data;
	
	fd = open(DEVICE_FILENAME, O_RDWR|O_NDELAY);
	if (fd < 0) {
		printf("Camera open fail 0x%x\n", fd);
		return -1;
	}

    //while(1) {
	ret = ioctl(fd, RCAM_GET_STATUS, &data);
	printf("Ioctl data:%d\n", data);
    //}
		
	close(fd);

	return 0;
}
