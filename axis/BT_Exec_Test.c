#include <stdio.h>

#define TC_BT_PATCHFILE  "/lib/modules/bt/patchram/TCM3800.hcd"
#define devicePath "/dev/ttyTCC1"
#define TC_BT_SOCKET_PATH "/var/run/"

int main(void)
{
    execlp("./bsa_server","bsa_server","-d", devicePath, "-r", "15", "-p", TC_BT_PATCHFILE, 0);
    return 0;
}

