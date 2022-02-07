
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <asm/termbits.h>

//#define __DEBUG_TEST__

int fd = 0;

#ifdef __DEBUG_TEST__
int parser(char *str)
{
    int total=0;

    total = strtoul(str, NULL, 16);

    return total;
}
#endif /*__DEBUG_TEST__*/

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

#ifdef __DEBUG_TEST__
    unsigned char cmd_cnt=0;
    unsigned char wbuf[128] = {0,};
#else
    unsigned char send_data[8] = {0xb5, 0x62, 0x0a, 0x0d, 0x00, 0x00, 0x17, 0x4f};
#endif /*__DEBUG_TEST__*/

    fd = open(TTYS3_PATH, O_RDWR|O_NOCTTY|O_SYNC);
    if(fd < 0) {
        printf("open faile 0x%x\n", fd);
    }

    setBaudrate(9600);
#ifdef __DEBUG_TEST__
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
#else
    /* B5 62 0A 0D 00 00 17 4F */
    printf("Tx : ");
    for(i=0; i<sizeof(send_data); i++)
        printf("0x%02x ", send_data[i]);
    printf("\n");

    write(fd, send_data, sizeof(send_data));
#endif /*__DEBUG_TEST__*/
    {
        unsigned char rxbuf[32]={0xFF,};
        unsigned char llcClk=0, clkDisabled=0, clkOscType=0, clkOscData=0;
        int state = 0;

        /* B5 62 0A 0D 18 00 FF FF FF FF ED FF FF FF FF FF FF FF BF FF FF FF F5 FF FF FF 6F FF FF FF 2B C3 */
        unsigned char rcv_data[6] = {0xb5, 0x62, 0x0a, 0x0d, 0x18, 0x00};

        printf("Rx : ");

        while(1) {
            read(fd, &rxbuf[state], 1);

            if(rxbuf[state] == rcv_data[state])
                state++;

            if(state >=4) {
                read(fd, &rxbuf[state], 28);
                for(i=0; i<32; i++) {
                    printf("%02x ", rxbuf[i]);
                    state=0;
                }
                break;
            }
        }
        printf("\n");

        llcClk = rxbuf[10];

        if(llcClk & 0x01) {
            printf("section read from eFuse\n");

            clkOscType = (llcClk >> 1) & 0x07;
            printf("clkOscType : 0x%02x\n", clkOscType);

            clkOscData = (llcClk >> 4) & 0x03;
            printf("clkOscData : 0x%02x\n", clkOscData);

            if(clkOscType == 7) {
                printf("OscType : XTO\n");
                if(clkOscData == 0x03)
                    printf("OscData : XTO autotuning\n");
                else
                    printf("OscData : all other values are reserved\n");
                
            } else if(clkOscType == 6) {
                printf("OscType : TCXO configuration 1\n");
                if(clkOscData == 0x03)
                    printf("OscData : LDO_X_OUT = 3.0V\n");
                else if(clkOscData == 0x02)
                    printf("OscData : LDO_X_OUT = 1.9V\n");
                else
                    printf("OscData : all other values are reserved\n");

            } else if(clkOscType == 5) {
                printf("OscType : TCXO configuration 2\n");
                if(clkOscData == 0x03)
                    printf("OscData : LDO_X_OUT = 2.6V\n");
                else if(clkOscData == 0x02)
                    printf("OscData : LDO_X_OUT = 1.6V\n");
                else
                    printf("OscData : all other values are reserved\n");

            } else if(clkOscType == 2) {
                printf("OscType : ultra-stable TCXO\n");
                if(clkOscData == 0x03)
                    printf("OscData : LDO_X_OUT = 3.0V\n");
                else if(clkOscData == 0x02)
                    printf("OscData : LDO_X_OUT = 1.9V\n");
                else if(clkOscData == 0x00)
                    printf("OscData : LDO_X_OUT = 2.6V\n");
                else
                    printf("OscData : all other values are reserved\n");
            } else {
                printf("values are reserved\n");
            }
        }
        else
            printf("section can't read from eFuse\n");
    }
    close(fd);

    return 0;
}

