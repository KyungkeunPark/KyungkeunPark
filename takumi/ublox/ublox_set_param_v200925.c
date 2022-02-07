#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <asm/termbits.h>

int devfd = 0;

int parser(char *str)
{
    int total=0;

    total = strtoul(str, NULL, 16);

    return total;
}

int checkSum(unsigned char *str, int size)
{
    unsigned char CK_A=0, CK_B=0;
    int i=0, ret=0;

    for(i=0; i<(size+4); i++) {
        CK_A = CK_A + str[i];
        CK_B = CK_B + CK_A;
    }

    if((str[size+4] == CK_A) && (str[size+5] == CK_B)) {
        printf("CheckSum OK [0x%x,0x%x]\n", CK_A, CK_B);
        ret = 1;
    } else {
        printf("CheckSum Fail : it should be [0x%x,0x%x], not [0x%x,0x%x]\n", CK_A, CK_B, str[size+4], str[size+5]);
        ret = 0;
    }
    return ret;
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

    tcflush ( devfd, TCIFLUSH );
    tcsetattr( devfd, TCSANOW, &newtio );

    return 0;
}

struct ublox_configure {
    unsigned char command[16];
    unsigned char head[2];
    unsigned char class;
    unsigned char id;
    unsigned char size;
    unsigned char payload[128];
    unsigned char checksum[2];
};
struct ublox_configure ublox_conf[5];

int param_write(unsigned char *payload, unsigned char size)
{
    write(devfd, payload, size);
    return 1;
}

int param_read(unsigned char *buf)
{
    int j=0, pos=0;
    unsigned char r_cfg[10] = {0xB5, 0x62, 0x05, 0x00, 0x02, 0x00, 0x06, 0x2F, 0x3C, 0x61};

    printf("Rx : ");

    while(1) {
        read(devfd, &buf[pos], 1);

        if(buf[pos] == r_cfg[pos]) {
            printf("%02x ", buf[pos]);
            pos++;
        }

        if(pos >= 3) {
            read(devfd, &buf[pos], 7);

            for(j=pos; j<10; j++) {
                printf("%02x ", buf[j]);
                pos=0;
            }

            /* Check the checksum */
            if(checkSum(&buf[2], buf[4]) != 1)
                return 0;

            return 1;
        }
    }
    return -1;
}


int main(int argc, char** argv)
{
    int i=0, cnt=0, state=-1, ret=0;
    char *TTYS3_PATH = "/dev/ttyAMA4";
    unsigned char cmd_cnt=0, checksumA=0, checksumB=0;

    int fd;
    //char path_name[32] = "/apps_data/";
    char path_name[32] = "./";
    char file_name[16];
    unsigned char wbuf[128] = {0,};

    char *ptr;
    struct stat file_info;

    devfd = open(TTYS3_PATH, O_RDWR|O_NOCTTY|O_SYNC);
    if(devfd < 0)
        printf("open faile 0x%x\n", devfd);

    setBaudrate(9600);

    if(argc != 2) {
        printf("There is no Configuration file\n");
        return 0;
    }

    strcpy(file_name, argv[1]);
    strcat(path_name, file_name);

    //printf("argc:%d\n", argc);
    //fd = fopen("/run/media/sda1/aaa.txt", O_RDONLY);
    fd = open(path_name, O_RDONLY);
    if(fd<0) {
        printf("%s open error (0x%x)\n", path_name, fd);
        return 0;
    }

    stat(path_name, &file_info);
    printf("\nFile Name:%s, File Size:%dBytes\n\n", path_name, file_info.st_size);

    ptr = malloc(sizeof(char)*file_info.st_size);
    if(ptr == NULL) {
        printf("memory alloc fail\n", ptr);
        return 0;
    }
    read(fd, ptr, file_info.st_size);

    close(fd);

    for(i=0; i<file_info.st_size; i++) {
        int offset=0, cnt=0, j=0;

        if((ptr[i+0] == '<') && (ptr[i+1] == 'U') && (ptr[i+2] == 'B') && (ptr[i+3] == 'X') &&
           (ptr[i+5] == 'C') && (ptr[i+6] == 'F') && (ptr[i+7] == 'G')) {

            if((ptr[i+9] == 'O') && (ptr[i+10] == 'T') && (ptr[i+11] == 'P')) {
                state = 0;
                strncpy(ublox_conf[state].command, ptr+i, 13);
                offset += (i+13+2);
                //printf("[0,%d] i<%d,0x%x> offset<%d,0x%x>\n", state, i, i, offset, offset);
                    
            } else if((ptr[i+9] == 'E') && (ptr[i+10] == 'S') && (ptr[i+11] == 'F') && (ptr[i+12] == 'L') && (ptr[i+13] == 'A')) {
                state = 1;
                strncpy(ublox_conf[state].command, ptr+i, 15);
                offset += (i+15+2);
                //printf("[1,%d] i<%d,0x%x> offset<%d,0x%x>\n", state, i, i, offset, offset);

            } else if((ptr[i+9] == 'E') && (ptr[i+10] == 'S') && (ptr[i+11] == 'F') && (ptr[i+12] == 'A') && (ptr[i+13] == 'L') && (ptr[i+14] == 'G')) {
                state = 2;
                strncpy(ublox_conf[state].command, ptr+i, 16);
                offset += (i+16+2);
                //printf("[2,%d] i<%d,0x%x> offset<%d,0x%x>\n", state, i, i, offset, offset);

            } else if((ptr[i+9] == 'E') && (ptr[i+10] == 'S') && (ptr[i+11] == 'F') && (ptr[i+12] == 'W') && (ptr[i+13] == 'T')) {
                state = 3;
                strncpy(ublox_conf[state].command, ptr+i, 15);
                offset += (i+15+2);
                //printf("[3,%d] i<%d,0x%x> offset<%d,0x%x>\n", state, i, i, offset, offset);

            } else if((ptr[i+9] == 'C') && (ptr[i+10] == 'F') && (ptr[i+11] == 'G')) {
                state = 4;
                strncpy(ublox_conf[state].command, ptr+i, 13);
                offset += (i+13+2);
                //printf("[4,%d] i<%d,0x%x> offset<%d,0x%x>\n", state, i, i, offset, offset);

            } else {
                state = -1;
                printf("command5:unknown\n");
            }

            if(state != -1) {
                printf("%s\n", ublox_conf[state].command);

                while(1) {
                    unsigned char tmp[2]={0,};

                    if((ptr[offset+0] == 0x0D) && (ptr[offset+1] == 0x0A)) {
                        offset += 4;
                        break;
                    }

                    if(ptr[offset] != 0x20) {
                        tmp[0] = ptr[offset+0];
                        tmp[1] = ptr[offset+1];
                        ublox_conf[state].payload[cnt] = parser(tmp);
                        offset+=2; cnt++;
                    } else
                        offset++;
                }
                i = offset-1;

                ublox_conf[state].size = cnt;

                for(j=0; j<ublox_conf[state].size; j++)
                    printf("0x%02x ", ublox_conf[state].payload[j]);
                printf("\n");

                /* Check the checksum */
                if(checkSum(&ublox_conf[state].payload[2], ublox_conf[state].payload[4]) != 1)
                    return 0;

                //printf("=> i:%d,0x%x offset:%d,0x%x\n", i, i, offset, offset);
                printf("\n");
            }
        }
    }
    free(ptr);
    printf("\n");

	if(state != 4) {
        printf("parser is fail (%d)\n", state);
        return 0;
    }

    {
        int index=0;//, j=0;

        /* UBX-CFG-OTP */
        // -> B5 62 06 41 0C 00 [00 00 03 1F CC 56 28 FF FF 76 73 FF] [A5 F4]
        // <- b5 62 05 01 02 00 [06 41] [4f 78]

        /* UBX-CFG-ESFLA */
        // -> B5 62 06 2F 2C 00 [00 05 00 00 00 00 E5 00 00 00 50 00 01 00 D2 00 00 00 3C 00 02 00 00 00 00 00 00 00 03 00 00 00 00 00 00 00 04 00 00 00 00 00 00 00] [B3 72]
        // <- B5 62 05 00 02 00 [06 2f] [3c 61]

        /* UBX-CFG-ESFALG */
        // -> B5 62 06 56 0C 00 [00 01 00 00 00 00 00 00 00 00 00 00] [69 1D]
        // <- B5 62 05 00 02 00 [06 56] [63 88]

        /* UBX-CFG-ESFWT */
        // -> B5 62 06 82 20 00 [00 40 00 00 00 00 00 00 00 00 00 00 01 00 00 00 00 00 00 10 00 00 00 00 00 00 00 00 00 00 00 00] [F9 82]
        // <- B5 62 05 00 02 00 [06 82] [8F B4]

        /* UBX-CFG-CFG */
        // -> B5 62 06 09 0D 00 [00 00 00 00 FF FF 00 00 00 00 00 00 03] [1D AB]
        // <- B5 62 05 01 02 00 [06 09] [17 40]

        for(index=0; index<5; index++) {

            int ret=0, retry=0, j=0;

            switch(index) {
                case 0:  printf("Tx (UBX-CFG-OTP)\n");    break;
                case 1:  printf("Tx (UBX-CFG-ESFLA)\n");  break;
                case 2:  printf("Tx (UBX-CFG-ESFALG)\n"); break;
                case 3:  printf("Tx (UBX-CFG-ESFWT)\n");  break;
                case 4:  printf("Tx (UBX-CFG-CFG)\n");    break;
                default: printf("Tx unKnown command\n");  break;
            }

            while(1) {
                unsigned char rxbuf[10] = {0,};
#if 0
                for(j=0; j<ublox_conf[index].size; j++)
                    printf("0x%02x ", ublox_conf[index].payload[j]);
                printf("\n");
#endif

                param_write(ublox_conf[index].payload, ublox_conf[index].size);

                ret = param_read(rxbuf);
                if(ret == 1) {
                    if((ublox_conf[index].payload[2] == rxbuf[6]) &&
                       (ublox_conf[index].payload[3] == rxbuf[7])) {
                        printf("***** Success *****\n\n");
                        break;
                    }
                }
                printf("Fail to write retry:%d\n", retry);
                printf("CheckSum is differnet [0x%02x, 0x%02x] [0x%02x, 0x%02x]\n\n", ublox_conf[index].payload[2], ublox_conf[index].payload[3], rxbuf[6], rxbuf[7]);
                retry++;

                if(retry > 30) {
                    printf("*******************************\n");
                    printf("Please run the Applicaton again\n");
                    printf("*******************************\n");
                    return 0;
                }
            }
        }
    }
    close(devfd);

    return 0;
}
