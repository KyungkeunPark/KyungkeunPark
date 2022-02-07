
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <asm/termbits.h>

int parser(char *str)
{
    int total=0;

    total = strtoul(str, NULL, 16);

    return total;
}

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

int main(int argc, char** argv)
{
    int fd, i=0, cnt=0;
    char *TTYS3_PATH = "/dev/ttyS3";
    unsigned char checksumA=0, checksumB=0;
    unsigned char cmd_cnt=0;

    unsigned char wbuf[128] = {0,};
    unsigned char rbuf[10] = {0,};

    fd = open(TTYS3_PATH, O_RDWR|O_NOCTTY|O_SYNC);

    setBaudrate(B115200);

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
    printf("send command\n");

    #if 1
    {
        //CFG-ANT      -> B5 62 06 13 04 00 01 00 F0 39 47 EA
        //             <- B5 62 05 01 02 00 06 13 21 4A

        //UB-CFG-ESFLA -> B5 62 06 56 0C 00 00 00 00 00 78 69 00 00 86 0B 00 00 DA EA
        //             <- B5 62 05 01 02 00 06 56 64 8D

        //UBX-CFG-ESFWT-> B5 62 06 82 20 00 00 40 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 0A 15 00 00 00 00 00 00 00 00 00 00 00 00 08 4F
        //             <- B5 62 05 01 02 00 06 82 90 B9

        //UBX-CFG-CFG  -> B5 62 06 09 0D 00 00 00 00 00 FF FF 00 00 00 00 00 00 03 1D AB
        //             <- B5 62 05 01 02 00 06 09 17 40
        //
        unsigned char buf=0;

        enum {
            HEADER1 = 0,
            HEADER2,
            CLASS,
            ID,
            LENGTH,
            PAYLOAD1,
            PAYLOAD2,
            PAYLOAD3,
            CHECKA,
            CHECKB,
            FINISH
        };
        int state = HEADER1;

        printf("ACK : ");

        while(1) {

            if(state == FINISH)
                break;

             read(fd, &buf, 1);
             switch(state) {
                 case HEADER1:
                     if(buf == 0xB5) {
                         printf("%02x ", buf);
                         state = HEADER2;
                     }
                     break;

                 case HEADER2:
                     if(buf == 0x62) {
                         printf("%02x ", buf);
                         state = CLASS;
                     }
                     break;

                 case CLASS:
                     if(buf == 0x05) {
                         printf("%02x ", buf);
                         state = ID;
                     }
                     break;

                 case ID:
                     if(buf == 0x01) {
                         printf("%02x ", buf);
                         state = LENGTH;
                     }
                     break;

                 case LENGTH:
                     if(buf == 0x02) {
                         printf("%02x ", buf);
                         state = PAYLOAD1;
                     }
                     break;

                 case PAYLOAD1:
                     if(buf == 0x00) {
                         printf("%02x ", buf);
                         state = PAYLOAD2;
                     }
                     break;

                 case PAYLOAD2:
                     if(buf == 0x06) {
                         printf("%02x ", buf);
                         state = FINISH;//PAYLOAD3;
                     }
                     break;
#if 0
                 case PAYLOAD3:
                     if(buf == 0x13) {
                         printf("%02x ", buf);
                         state = CHECKA;
                     }
                     break;

                 case CHECKA:
                     if(buf == 0x21) {
                         printf("%02x ", buf);
                         state = CHECKB;
                     }
                     break;

                 case CHECKB:
                     if(buf == 0x4A) {
                         printf("%02x\n", buf);
                         state = FINISH;
                     }
                     break;
#endif
                 default:
                     printf(".");
                     break;
            }
        }
    }
    #endif

    printf("Success\n");
    close(fd);

    return 0;
}
