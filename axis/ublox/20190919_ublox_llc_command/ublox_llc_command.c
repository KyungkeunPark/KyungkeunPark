
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <asm/termbits.h>

int setBaudrate(int speed)
{
    struct termios tty;

    //if (tcgetattr(nFileDescriptor, &tty) < 0)
    //{
    //    DBG_ERROR;
    //    DBG(_T("Error from tcsetattr: %d\n"), strerror(errno));
    //    DBG_CLEAR;
    //    return -1;
    //}

    cfsetospeed(&tty, (speed_t)speed);
    cfsetispeed(&tty, (speed_t)speed);

    tty.c_cflag |= (CLOCAL | CREAD); /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;      /* 8-bit characters */
    tty.c_cflag &= ~PARENB;  /* no parity bit */
    tty.c_cflag &= ~CSTOPB;  /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS; /* no hardware flowcontrol */

    /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_oflag &= ~OPOST;

    /* fetch bytes as they become available */
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;
/*
    if (tcsetattr(nFileDescriptor, TCSANOW, &tty) != 0)
    {
        DBG_ERROR;
        DBG(_T("Error from tcsetattr: %d\n"), strerror(errno));
        DBG_CLEAR;
        return -1;
    }
*/
    return 0;
}

int main(void)
{
    int fd, i=0;
    unsigned char rbuf[136];
    char *TTYS3_PATH = "/dev/ttyS3";
    unsigned char checksumA=0, checksumB=0;

    fd = open(TTYS3_PATH, O_RDWR|O_NOCTTY);

    setBaudrate(B115200);
    
    /* B5 62 06 41 0C 00 00 00 03 1F DA 93 DC 22 FF 76 EF FF 43 EE */
    {
        unsigned char wbuf[20] = {0,};

        wbuf[0] = 0xB5;
        wbuf[1] = 0x62;
        wbuf[2] = 0x06;
        wbuf[3] = 0x41;
        wbuf[4] = 0x0C;
        wbuf[5] = 0x00;
        wbuf[6] = 0x00;
        wbuf[7] = 0x00;
        wbuf[8] = 0x03;
        wbuf[9] = 0x1F;
        wbuf[10] = 0xDA;
        wbuf[11] = 0x93;
        wbuf[12] = 0xDC;
        wbuf[13] = 0x22;
        wbuf[14] = 0xFF;
        wbuf[15] = 0x76;
        wbuf[16] = 0xEF;
        wbuf[17] = 0xFF;
        wbuf[18] = 0xFF;
        wbuf[19] = 0xFF;

        for(i=2; i<18; i++) {
            checksumA = checksumA + wbuf[i];
            checksumB = checksumB + checksumA;
        }
        wbuf[18] = checksumA;
        wbuf[19] = checksumB;

        for(i=0; i<20; i++) {
            printf("0x%02x ", wbuf[i]);
        }
        printf("\n");
        printf("\n");

        write(fd, wbuf, sizeof(wbuf));
    }

#if 0
    read(fd, rbuf, sizeof(rbuf));

    for(i=0; i<sizeof(rbuf); i++) {
        printf("0x%02x ", rbuf[i]);
    }
    printf("\n");

    checksumA = 0;
    checksumB = 0;

    for(i=2; i<134; i++) {
        checksumA = checksumA + rbuf[i];
        checksumB = checksumB + checksumA; 
    }
    printf("0x%02x, 0x%02x\n", checksumA, checksumB);
#endif
    close(fd);

    return 0;
}
