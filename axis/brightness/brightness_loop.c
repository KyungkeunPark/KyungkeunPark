
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
	//char data[5] = {30, 10, 20, 1, 40};
	char data[2] = {0, 26};
	char cmd[4];
	char *brightness_path = "/sys/class/tcc_dispman/tcc_dispman/color_enhance_lcd_brightness";
	char *contrast_path = "/sys/class/tcc_dispman/tcc_dispman/color_enhance_lcd_contrast";

	fd = open(brightness_path, O_RDWR);
	
	while (1) {

		sprintf(cmd, "%d", data[value]);

		printf("BRIGHTNESS%d = %d\n", value, data[value]);

		write(fd, cmd, strlen(cmd));
		write(fd, cmd, strlen(cmd));
		write(fd, cmd, strlen(cmd));

		cnt++;
		value = cnt%sizeof(data);

		sleep(2);
	}

	close(fd);

	return 0;
}

