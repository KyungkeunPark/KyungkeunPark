
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <asm/termbits.h>
#include <memory.h>

int fd = 0;

int setBaudrate(int speed)
{
    struct termios newtio;

    memset(&newtio, 0, sizeof(newtio));

    /*
       CS8    : 8n1 (8bit,no parity,1 stopbit)
       CLOCAL : local connection, no modem contol
       CREAD  : enable receiving characters
     */
    newtio.c_cflag = CS8 | CLOCAL | CREAD; // NO-rts/cts

    switch( speed ) {
        case 115200 : newtio.c_cflag |= B115200; break;
        case 57600 :  newtio.c_cflag |= B57600;  break;
        case 38400 :  newtio.c_cflag |= B38400;  break;
        case 19200 :  newtio.c_cflag |= B19200;  break;
        case 9600 :   newtio.c_cflag |= B9600;   break;
        case 4800 :   newtio.c_cflag |= B4800;   break;
        case 2400 :   newtio.c_cflag |= B2400;   break;
        default :     newtio.c_cflag |= B115200; break;
    }

    newtio.c_oflag = 0;
    newtio.c_lflag = ICANON;
    newtio.c_iflag = IGNPAR; // non-parity

    newtio.c_cc[VTIME] = 0;/* inter-character timer unused */
    //newtio.c_cc[VMIN] = 1;
    newtio.c_cc[VMIN] = 0; /*blocking read until 1 character arrives*/

    tcflush ( fd, TCIFLUSH );
    tcsetattr( fd, TCSANOW, &newtio );

    return 0;
}

int main(int argc, char** argv)
{
    int i=0;
    char *TTYS3_PATH = "/dev/ttyS3";

    /* send command to get Navigation PVT Solution */
    unsigned char send_data[8] = {0xb5, 0x62, 0x01, 0x07, 0x00, 0x00, 0x08, 0x19};

    fd = open(TTYS3_PATH, O_RDWR|O_NOCTTY|O_SYNC);
    if(fd < 0) {
        printf("open faile 0x%x\n", fd);
    }

    setBaudrate(9600);

    printf("Tx : ");
    for(i=0; i<sizeof(send_data); i++)
        printf("0x%02x ", send_data[i]);
    printf("\n");

    write(fd, send_data, sizeof(send_data));
    {
        unsigned char rxbuf[256]={0xFF,};
        int state = 0;
        int CheckSumSize = 2;
        int headerSize = 6;

        unsigned char rcv_data[6] = {0xb5, 0x62, 0x01, 0x07, 0x5c, 0x00};
        int rcv_size=rcv_data[4]+CheckSumSize;

        printf("Rx : ");

        while(1) {
            read(fd, &rxbuf[state], 1);

            if(rxbuf[state] == rcv_data[state]) {
                printf("%02x ", rxbuf[state]);
                state++;
            }

            if(state >= 6) {
                read(fd, &rxbuf[state], rcv_size);

                for(i=state; i<rcv_size+headerSize; i++) {
                    printf("%02x ", rxbuf[i]);
                    state=0;
                }
                break;
            }
        }
        printf("\n\n");

        printf("FixType : [%d] ", rxbuf[26]);

        switch(rxbuf[26]) {
            case 0:
                printf("no fix\n");
                break;
            case 1:
                printf("dead reckoning only\n");
                break;
            case 2:
                printf("2D-fix\n");
                break;
            case 3:
                printf("3D-fix\n");
                break;
            case 4:
                printf("GNSS + dead reckoning combined\n");
                break;
            case 5:
                printf("time only fix\n");
                break;
            default:
                printf("unknown\n");
                break;

        }
        printf("\n");
    }
    close(fd);

    return 0;
}

