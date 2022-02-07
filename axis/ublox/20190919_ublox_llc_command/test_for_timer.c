
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <asm/termbits.h>
 
#define INTERVAL 10
int completed = 0;

int setBaudrate(int speed)
{
    struct termios tty;

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

    return 0;
}

void ack_timer(int signo)
{
    printf("Cna not get UBX return value\n");
    completed = 1;
}
 
int main(void)
{
    int fd, i=0;
    unsigned char rbuf=0;
    char *TTYS3_PATH = "/dev/ttyS3";
    unsigned char checksumA=0, checksumB=0;
    int status = 0;
    unsigned char ack_data[10] = {0xB5, 0x62, 0x05, 0x01, 0x02, 0x00, 0x06, 0x41, 0x4F, 0x78};
    struct sigaction act;

    fd = open(TTYS3_PATH, O_RDWR|O_NOCTTY);

    setBaudrate(B115200);
    
    /* B5 62 06 41 0C 00 00 00 03 1F DA 93 DC 22 FF 76 EF FF 43 EE */
    {
        unsigned char wbuf[20] = {0,};

        /* header */
        wbuf[0] = 0xB5; wbuf[1] = 0x62;
        /* class */
        wbuf[2] = 0x06;
        /* id */
        wbuf[3] = 0x41;
        /* length */
        wbuf[4] = 0x0C; wbuf[5] = 0x00;
        /* payload */
        wbuf[6] = 0x00; wbuf[7] = 0x00; wbuf[8] = 0x03; wbuf[9] = 0x1F;
        wbuf[10]= 0xDA; wbuf[11]= 0x93; wbuf[12]= 0xDC; wbuf[13]= 0x22;
        wbuf[14]= 0xFF; wbuf[15]= 0x76; wbuf[16]= 0xEF; wbuf[17]= 0xFF;
        /* checksum */
        wbuf[18]= 0xFF; wbuf[19]= 0xFF;

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

    /* if there is no return for 10sec, it ends */
    act.sa_handler = ack_timer;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
 
    sigaction(SIGALRM, &act, 0);
    alarm(INTERVAL);
 
    /* Get Ack */
    while(1) {

        if(status >= 10) {
            printf("\nWrite Success !!!\n");
            break;
        }
        if(completed == 1) {
            printf("\nTimeOut...\n");
            break;
        }

        read(fd, &rbuf, 1);

        /* NAK */
        if((status == 3) && (rbuf == 0x00)) {
            printf("There was a problem (NAK)\n");
            break;
        }

        /* ACK */
        if(rbuf == ack_data[status]) {
            printf("0x%x ", rbuf);
            status++;
        }
    }
    close(fd);

    return 0;
}
