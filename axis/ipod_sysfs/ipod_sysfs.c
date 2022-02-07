
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
	int fd;
	int cnt=0, value=0;
	unsigned char buf[6];
	char *ipod_path= "/sys/devices/auth-cp/tcc_cp";

	fd = open(ipod_path, O_RDWR);

	read(fd, buf, strlen(buf));

	printf("0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n", buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
	printf("%s\n", buf);

	close(fd);

	return 0;
}

