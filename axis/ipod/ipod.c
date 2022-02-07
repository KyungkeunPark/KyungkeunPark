
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE_FILENAME "/dev/tcc-cp"

#define CP_DEV_MAJOR_NUM 237
#define CP_DEV_MINOR_NUM 1

#define IOCTL_CP_CTRL_INIT                 _IO(CP_DEV_MAJOR_NUM, 100)
#define IOCTL_CP_CTRL_PWR                  _IO(CP_DEV_MAJOR_NUM, 101)
#define IOCTL_CP_CTRL_RESET                _IO(CP_DEV_MAJOR_NUM, 102)
#define IOCTL_CP_CTRL_ALL                  _IO(CP_DEV_MAJOR_NUM, 103)
#define IOCTL_CP_GET_VERSION               _IO(CP_DEV_MAJOR_NUM, 104)
#define IOCTL_CP_GET_CHANNEL               _IO(CP_DEV_MAJOR_NUM, 105)
#define IOCTL_CP_SET_STATE                 _IO(CP_DEV_MAJOR_NUM, 106)
#define IOCTL_CP_GET_STATE                 _IO(CP_DEV_MAJOR_NUM, 107)
#define IOCTL_CP_GET_WITH_IAP2             _IO(CP_DEV_MAJOR_NUM, 108)
#define IOCTL_CP_GET_DEVICE_ID             _IO(CP_DEV_MAJOR_NUM, 109)
#define IOCTL_CP_GET_AUTH_PROTOCOL_VERSION _IO(CP_DEV_MAJOR_NUM, 110)


int main(void)
{
    int dev, ret;
    int id, auth_ver;

    dev = open(DEVICE_FILENAME, O_RDWR|O_NDELAY);
    if (dev < 0) {
        printf("Elmos Fiel open fail 0x%\n", dev);
        return -1;
    }

    printf("Application : Ioctl\n");
    ret = ioctl(dev, IOCTL_CP_GET_DEVICE_ID, &id);
    //printf("ID:0x%x\n", id);

    ret = ioctl(dev, IOCTL_CP_GET_AUTH_PROTOCOL_VERSION, &auth_ver);
    //printf("AUTH Ver:0x%x\n", auth_ver);
    
	printf("Application : close.\n");
    ret = close(dev);

    printf("\n"); printf("\n"); printf("\n"); printf("\n");
    
    if (id == 0x200) {

    printf(" ********** *********        ***********    *********     ********* \n");
	printf(" ********** **********       ***********   ***********   ***********\n");
	printf("    ***     **      ***              ***   ***     ***   ***     ***\n");
	printf("    ***     **       **  **           **   **       **   **       **\n");
	printf("    ***     **       **  **           **   **       **   **       **\n");
	printf("    ***     **       **  **          ***   **       **   **       **\n");
	printf("    ***     **       **       **********   **       **   **       **\n");
	printf("    ***     **       **      ***********   **       **   **       **\n");
	printf("    ***     **       **  **  ***           **       **   **       **\n");
	printf("    ***     **       **  **  **            **       **   **       **\n");
	printf("    ***     **       **  **  **            **       **   **       **\n");
	printf("    ***     **      ***      ***           ***     ***   ***     ***\n");
	printf(" *********  **********       ***********   ***********   ***********\n");
	printf(" *********  *********         **********    *********     ********* \n");

    } else {
 
    printf(" ********** *********         ********* \n");
	printf(" ********** **********       ***********\n");
	printf("    ***     **      ***      ***     ***\n");
	printf("    ***     **       **  **  **       **\n");
	printf("    ***     **       **  **  **       **\n");
	printf("    ***     **       **  **  **       **\n");
	printf("    ***     **       **      **       **\n");
	printf("    ***     **       **      **       **\n");
	printf("    ***     **       **  **  **       **\n");
	printf("    ***     **       **  **  **       **\n");
	printf("    ***     **       **  **  **       **\n");
	printf("    ***     **      ***      ***     ***\n");
	printf(" *********  **********       ***********\n");
	printf(" *********  *********         ********* \n");

    }

    printf("\n"); printf("\n"); printf("\n"); printf("\n");

	if (auth_ver == 0x200) {

    printf(" **     **  **********  *********        ***********    *********     ********* \n");
	printf(" **     **  **********  **********       ***********   ***********   ***********\n");
	printf(" **     **  **          **     ****              ***   ***     ***   ***     ***\n");
	printf(" **     **  **          **      ***  **           **   **       **   **       **\n");
	printf(" **     **  **          **       **  **           **   **       **   **       **\n");
	printf(" **     **  **          **     ****  **          ***   **       **   **       **\n");
	printf(" **     **  *********   **********       ***********   **       **   **       **\n");
	printf(" **     **  *********   **********       ***********   **       **   **       **\n");
	printf(" **     **  **          **     ****      ***           **       **   **       **\n");
	printf(" **     **  **          **       **  **  **            **       **   **       **\n");
	printf(" **     **  **          **       **  **  **            **       **   **       **\n");
	printf(" ***   ***  **          **       **  **  ***           ***     ***   ***     ***\n");
	printf("   ** **    **********  **       **      ***********   ***********   ***********\n");
	printf("     *      **********  **       **      ***********    *********     ********* \n");

    } else {

    printf(" **     **  **********  *********         ********* \n");
	printf(" **     **  **********  **********       ***********\n");
	printf(" **     **  **          **     ****      ***     ***\n");
	printf(" **     **  **          **      ***  **  **       **\n");
	printf(" **     **  **          **       **  **  **       **\n");
	printf(" **     **  **          **     ****  **  **       **\n");
	printf(" **     **  *********   **********       **       **\n");
	printf(" **     **  *********   **********       **       **\n");
	printf(" **     **  **          **     ****      **       **\n");
	printf(" **     **  **          **       **  **  **       **\n");
	printf(" **     **  **          **       **  **  **       **\n");
	printf(" ***   ***  **          **       **  **  ***     ***\n");
	printf("   ** **    **********  **       **      ***********\n");
	printf("     *      **********  **       **       ********* \n");

    }
	
    printf("\n"); printf("\n"); printf("\n"); printf("\n");
    
    return 0;
}

