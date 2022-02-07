#include <stdio.h>
#include <stdlib.h>

float vbus_v_buf[10] = {0,};
int buf_cnt = 0;
int vbus_chk_cnt = 0;
int vbus_v_buf_full = 0;

void vbus_v_push(float data)
{
    vbus_v_buf[buf_cnt] = data;

    if (buf_cnt >= 9)
        buf_cnt = 0;
    else
        buf_cnt++;

    if (vbus_chk_cnt >= 9) {
        if (vbus_v_buf_full == 0) {
            vbus_v_buf_full = 1;
        }
    } else
        vbus_chk_cnt++; 

    return;
}

float vbus_v_pop(void)
{
    float vbus_v_average = 0;
    float vbus_v_total = 0;
    int i=0;

    if (vbus_v_buf_full) {
        for (i=0; i<10; i++) {
            vbus_v_total += vbus_v_buf[i];
        }
        vbus_v_average = vbus_v_total / 10;
    } else
        printf("vbus buf not full\n");

    return vbus_v_average;
}

void vbus_v_dump(void)
{
    int i=0;

    for(i=0; i<10; i++)
        printf("[%f]", vbus_v_buf[i]);
    
    printf("\n");
    return;    
}

int main(void)
{
    vbus_v_push(1.1); printf("-1- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-2- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-3- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-4- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-5- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-6- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-7- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-8- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-9- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-10- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-11- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-12- %f\n", vbus_v_pop());  vbus_v_dump();
    vbus_v_push(1.1); printf("-13- %f\n", vbus_v_pop());  vbus_v_dump();

    return 0;
}






















