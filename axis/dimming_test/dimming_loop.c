
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
	unsigned char data[6] = {200, 100, 250, 50, 150, 50};
	unsigned char cmd[4];

	char *backlight_path = "/sys/class/leds/lcd-backlight/brightness";

	fd = open(backlight_path, O_RDWR);
	
	while (1) {

		sprintf(cmd, "%d", data[value]);
	
		printf("DIMMING:%d = %d\n", value, data[value]);

		write(fd, cmd, strlen(cmd));

		cnt++;
		value = cnt%6;

		sleep(2);
	}

	close(fd);

	return 0;
}

