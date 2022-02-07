
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <asm/termbits.h>

char *TTYS3_PATH = "/dev/ttyS3";
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

struct sigaction act;
#define INTERVAL 10
int completed = 0;
void ack_timer(int signo)
{
    printf("Waited %d seconds\n", INTERVAL);
    completed = 1;
}

unsigned int change_baudrate(void)
{
    int i=0;
    unsigned char send_data[28] = 
             {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xC2, 0x01, 0x00, 0x07, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0xC2, 0x86};
    unsigned char rxbuf[10] = {0xFF,};

    setBaudrate(9600);

    printf("Tx1 : ");
    for(i=0; i<sizeof(send_data); i++)
        printf("0x%02x ", send_data[i]);
    printf("\n");

    write(fd, send_data, sizeof(send_data));

    while(!completed);
    completed = 0;

    /* if there is no return for 10sec, it ends */
    act.sa_handler = ack_timer;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
 
    sigaction(SIGALRM, &act, 0);
    alarm(INTERVAL);

    setBaudrate(115200);

    printf("Tx2 : ");
    for(i=0; i<sizeof(send_data); i++)
        printf("0x%02x ", send_data[i]);
    printf("\n");

    write(fd, send_data, sizeof(send_data));
    printf("ACK : ");
#if 1
    {
        int state = 0;
        unsigned char ack_data[10] = {0xB5, 0x62, 0x05, 0x01, 0x02, 0x00, 0x06, 0x00, 0x0E, 0x37};

        while(1) {
            read(fd, &rxbuf[state], 1);

            if(rxbuf[state] == ack_data[state])
                state++;

            if(state >=3) {
                read(fd, &rxbuf[state], 7);
                for(i=0; i<10; i++) {
                    printf("%02x ", rxbuf[i]);
                    state=0;
                }
                break;
            }
        }
        printf("\n\n");
    }
#endif

    while(!completed);
    completed = 0;

    return rxbuf[3];
}

int main(int argc, char** argv)
{
    int i=0;
    unsigned char send_data[8] = {0xb5, 0x62, 0x0a, 0x24, 0x00, 0x00, 0x2e, 0x94};

    /* if there is no return for 10sec, it ends */
    act.sa_handler = ack_timer;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
 
    sigaction(SIGALRM, &act, 0);
    alarm(INTERVAL);

    fd = open(TTYS3_PATH, O_RDWR|O_NOCTTY|O_SYNC);
    if(fd < 0) {
        printf("open faile 0x%x\n", fd);
    }
    change_baudrate();

    setBaudrate(115200);

    /* B5 62 0A 24 00 00 2E 94 */
    printf("Tx : ");
    for(i=0; i<sizeof(send_data); i++)
        printf("0x%02x ", send_data[i]);
    printf("\n");

    write(fd, send_data, sizeof(send_data));
    {
        unsigned char rxbuf[32]={0xFF,};
        unsigned char PIO_15=0;
        int state = 0;

        unsigned char rcv_data[6] = {0xb5, 0x62, 0x0a, 0x24, 0x13, 0x00};

        printf("Rx : ");

        while(1) {
            read(fd, &rxbuf[state], 1);

            if(rxbuf[state] == rcv_data[state])
                state++;

            if(state >=4) {
                read(fd, &rxbuf[state], 23);
                for(i=0; i<27; i++) {
                    printf("%02x ", rxbuf[i]);
                    state=0;
                }
                break;
            }
        }
        printf("\n");

        /*B5 62 0A 24 13 00 01 01 P0 P1 P2 P3 P4 P5 P6 P7 P8 P9 P10 P11 P12 P13 P14 P15 P16 A B*/
        PIO_15 = rxbuf[23];
        if(PIO_15 == 0x04) //High
            printf("reverse detect is OFF\n");
        else if(PIO_15 == 0x05) //LOW
            printf("reverse detect is ON\n");
        else
            printf("unknown\n");
    }
    close(fd);

    return 0;
}

