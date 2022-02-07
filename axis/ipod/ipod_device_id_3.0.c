#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

#define I2C_READ_MSG_NUMBER 2
#define DEV_I2C_CHANNEL	    "/dev/i2c-1"
#define I2C_SLAVE_ADDR	    (0x20 >> 1)
#define I2C_RETRY_NUM	    30

static int i2c_read(int i2c, int slaveaddr, char *ptr, unsigned char subaddr, size_t len)
{
	int ret;
	struct i2c_msg msgs[I2C_READ_MSG_NUMBER];
	struct i2c_rdwr_ioctl_data	data;

	if((i2c > 0) && (ptr != NULL) && (len != 0)) {
		memset(msgs, 0x00, sizeof(struct i2c_msg)*I2C_READ_MSG_NUMBER);
		memset(&data, 0x00, sizeof(struct i2c_rdwr_ioctl_data));
		memset(ptr, 0x00, len);

        /* Write */
		msgs[0].addr  = slaveaddr;
		//msgs[0].flags = 0;
		msgs[0].flags |= I2C_M_STOP;
		msgs[0].len   = 1;
		msgs[0].buf   = &subaddr;

        /* Read */
		msgs[1].addr  = slaveaddr;
		msgs[1].flags = I2C_M_RD;
		msgs[1].len   = len;
		msgs[1].buf   = ptr;

		data.msgs     = msgs;
		data.nmsgs    = I2C_READ_MSG_NUMBER;

		ret = ioctl(i2c, I2C_RDWR, &data);
		if(ret < 0)
			goto exit;
	} else
		ret = -1;

exit:
	if(ret < 0)
		printf("%s() ret : %d, ERROR : %d, %s\n", __FUNCTION__, ret, errno, strerror(errno));

	return ret;
}

void main(int argc, const char **argv)
{
    int i2c, ret;
    unsigned char data[8]={0xFF,};

	i2c = open(DEV_I2C_CHANNEL, O_RDWR);
	if(i2c < 0) {
		printf("Can't open %s ERROR : %d, %s\n", DEV_I2C_CHANNEL, errno, strerror(errno));
		goto exit;
	}

	ret = ioctl(i2c, I2C_SLAVE, I2C_SLAVE_ADDR);
	if(ret < 0){
		printf("ioctl(I2C_SLAVE) ERROR : %d, %s\n", errno, strerror(errno));
		goto exit;
	}

	ret = ioctl(i2c, I2C_RETRIES, I2C_RETRY_NUM);
	if(ret < 0){
		printf("ioctl(I2C_RETRIES) ERROR : %d, %s\n", errno, strerror(errno));
		goto exit;
	}

    /*Get Device Version, default 0x07*/
	ret = i2c_read(i2c, I2C_SLAVE_ADDR, data, 0x00, 1);
    if(ret < 0)
	    printf("I2C Read Error %d\n", ret);
	printf("Device Version:[0x%02x]\n", data[0]);

    /*Get Authentication Revision, default 0x00*/
	ret = i2c_read(i2c, I2C_SLAVE_ADDR, data, 0x01, 1);
    if(ret < 0)
	    printf("I2C Read Error %d\n", ret);
	printf("Authentication Revision:[0x%02x]\n", data[0]);

    /*Get Authentication Protocol Major Version, default 0x03*/
	ret = i2c_read(i2c, I2C_SLAVE_ADDR, data, 0x02, 1);
    if(ret < 0)
	    printf("I2C Read Error %d\n", ret);
	printf("Authentication Protocol Major Version : [0x%02x]\n", data[0]);

    /*Get Authentication Protocol Minor Version, default 0x03*/
	ret = i2c_read(i2c, I2C_SLAVE_ADDR, data, 0x03, 1);
    if(ret < 0)
	    printf("I2C Read Error %d\n", ret);
	printf("Authentication Protocol Minor Version : [0x%02x]\n", data[0]);

    /*Get Device ID, default 0x00 0x00 0x03 0x00*/
	ret = i2c_read(i2c, I2C_SLAVE_ADDR, data, 0x04, 4);
    if(ret < 0)
	    printf("I2C Read Error %d\n", ret);
	printf("Device ID : [0x%02x][0x%02x][0x%02x][0x%02x]\n", data[0], data[1], data[2], data[3]);

    /*Get Error Code, defalut 0x00*/
	ret = i2c_read(i2c, I2C_SLAVE_ADDR, data, 0x05, 1);
    if(ret < 0)
	    printf("I2C Read Error %d\n", ret);
	printf("Error Code : [0x%02x]\n", data[0]);

exit:
	if(i2c > 0)
		close(i2c);

	return;
}

