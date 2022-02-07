#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define PATH "/sys/devices/79080000.tcc_cm_ctrl/rcam_gear_status"

int main(void)
{
    int fd, epfd;
    char buf;
    struct epoll_event ev;
    int res;

    fd = open(PATH, O_RDONLY);

    epfd = epoll_create(1);

    ev.events = EPOLLIN|EPOLLET;
    ev.data.fd = fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    while (1) {
        res = epoll_wait(epfd, &ev, 1, 1000);
        if (res == 0)
            continue;

        else if (res < 0)
            printf("Events error : %d\n", res);

        else {
            res = read(fd, &buf, sizeof(buf));
            lseek(fd, 0, SEEK_SET);
            /* reverse gear value */
            printf("gear : 0x%x\n", buf);
        }
    }

    close(fd);
}

