
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

    unsigned char sw_version[23] = {0,};
    unsigned char hw_version[9] = {0,};
    unsigned char fw_version[15] = {0,};
    unsigned char protocol_ver[14] = {0,};

    /* send command to get version */
    // KK : 0xb5 0x62 0x01 0x12 0x00 0x00 0x13 0x3A
    unsigned char send_data[8] = {0xb5, 0x62, 0x0a, 0x04, 0x00, 0x00, 0x0e, 0x34};

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

        // KK : 0xb5 0x62 0x01 0x12 0x24 0x00 
        unsigned char rcv_data[6] = {0xb5, 0x62, 0x0a, 0x04, 0xa0, 0x00};
        int rcv_size=160;

        printf("Rx : ");

        while(1) {
            read(fd, &rxbuf[state], 1);

            if(rxbuf[state] == rcv_data[state]) {
                printf("%02x ", rxbuf[state]);
                state++;
            }

            if(state >= 4) {
                read(fd, &rxbuf[state], rcv_size);

                for(i=state; i<rcv_size; i++) {
                    printf("%02x ", rxbuf[i]);
                    state=0;
                }
                break;
            }
        }
        printf("\n");

        // KK : you can get a data you want from the received 36 bytes from the codel below.
        memset(&sw_version[0], 0xFF, sizeof(sw_version));
        memset(&hw_version[0], 0xFF, sizeof(hw_version));
        memset(&fw_version[0], 0xFF, sizeof(fw_version));
        memset(&protocol_ver[0], 0xFF, sizeof(protocol_ver));

        if((rxbuf[6] == 'R') && (rxbuf[7] == 'O') && (rxbuf[8] == 'M')) {
            memcpy(&sw_version[0],   &rxbuf[6],   sizeof(sw_version));
            memcpy(&hw_version[0],   &rxbuf[36],  sizeof(hw_version));
            memcpy(&fw_version[0],   &rxbuf[46],  sizeof(fw_version));
            memcpy(&protocol_ver[0], &rxbuf[76],  sizeof(protocol_ver));

            printf("This firmware is not for ADR\n");

        } else if((rxbuf[6] == 'E') && (rxbuf[7] == 'X') && (rxbuf[8] == 'T')) {
            memcpy(&sw_version[0],   &rxbuf[6],   sizeof(sw_version));
            memcpy(&hw_version[0],   &rxbuf[36],  sizeof(hw_version));
            memcpy(&fw_version[0],   &rxbuf[76],  sizeof(fw_version));
            memcpy(&protocol_ver[0], &rxbuf[106], sizeof(protocol_ver));

            printf("This firmware is for ADR\n");
        }

        printf("SW Ver : %s\n", sw_version);
        printf("HW Ver : %s\n", hw_version);
        printf("FW Ver : %s\n", fw_version);
        printf("Protocol Ver : %s\n", protocol_ver);
    }
    close(fd);

    return 0;
}

