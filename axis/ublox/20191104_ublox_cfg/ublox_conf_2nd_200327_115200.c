
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <asm/termbits.h>

int fd = 0;

int parser(char *str)
{
    int total=0;

    total = strtoul(str, NULL, 16);

    return total;
}

int setBaudrate(int speed)
{
    struct termios newtio;

    memset(&newtio, 0, sizeof(newtio));

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

    newtio.c_cc[VTIME] = 0;
    //newtio.c_cc[VMIN] = 1;
    newtio.c_cc[VMIN] = 0;

    tcflush ( fd, TCIFLUSH );
    tcsetattr( fd, TCSANOW, &newtio );

    return 0;
}

int main(int argc, char** argv)
{
    int i=0, cnt=0;
    char *TTYS3_PATH = "/dev/ttyS3";
    unsigned char cmd_cnt=0;
    unsigned char checksumA=0, checksumB=0;

    unsigned char wbuf[128] = {0,};

    fd = open(TTYS3_PATH, O_RDWR|O_NOCTTY|O_SYNC);
    if(fd < 0) {
        printf("open faile 0x%x\n", fd);
    }

    setBaudrate(115200);

    cmd_cnt = argc-1;

    printf("Send cmd : ");
    for(i=1; i<argc; i++) {
        wbuf[i-1] = parser(argv[i]);
        printf("%s ", argv[i]);
    }
    printf("\n");

    if((wbuf[0] != 0xb5) || (wbuf[1] != 0x62)) {
        printf("Please check the command\n");
        close(fd);
        return 0;
    }

    write(fd, wbuf, cmd_cnt);
    {
        // UBX-CFG-ANT
        // -> ublox_conf_2nd_200324 B5 62 06 13 04 00 01 00 F0 39 47 EA
        // <- B5 62 05 01 02 00 06 13 21 4A

        // UBX-CFG-ESFLA
        // -> ublox_conf_2nd_200324 B5 62 06 56 0C 00 00 00 00 00 78 69 00 00 86 0B 00 00 DA EA
        // <- B5 62 05 01 02 00 06 56 64 8D

        // UBX-CFG-ESFWT
        // -> ublox_conf_2nd_200324 B5 62 06 82 20 00 00 40 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 0A 15 00 00 00 00 00 00 00 00 00 00 00 00 08 4F
        // <- B5 62 05 01 02 00 06 82 90 B9

        // UBX-CFG-CFG
        // -> ublox_conf_2nd_200324 B5 62 06 09 0D 00 00 00 00 00 FF FF 00 00 00 00 00 00 03 1D AB
        // <- B5 62 05 01 02 00 06 09 17 40

        // UBX-CFG-PRT 
        // -> B5 62 06 00 14 00 01 00 00 00 D0 08 00 00 00 C2 01 00 01 00 01 00 00 00 00 00 B8 42

        unsigned char buf=0;
        unsigned char rxbuf[10]={0xFF,};

        int state = 0;
        unsigned char ack_data[6] = {0xb5, 0x62, 0x05, 0x01, 0x02, 0x00};

        printf("ACK: ");

        while(1) {
            read(fd, &buf, 1);

            if(buf == ack_data[state]) {
                printf("%02x ", buf);
                state++;
            }
            if(state >=3) {
                read(fd, rxbuf, 7);
                for(i=0; i<7; i++) {
                    printf("%02x ", rxbuf[i]);
                    state=0;
                }
                break;
            }
        }
        printf("\n");

        if(rxbuf[0] == 0x01) {
            printf("Write Success !!!\n");
        } else {
            printf("Write Fail (0x%02x)\n", rxbuf[0]);
        }
    }
    close(fd);

    return 0;
}
