#include <stdio.h>

/*difinition function pointer*/
typedef int (*calcFuncPtr)(int, int);

int plus  (int f, int s) { return f+s; }
int minu  (int f, int s) { return f-s; }
int mult  (int f, int s) { return f*s; }
int divi  (int f, int s) { return f/s; }

int calculator(int f, int s, calcFuncPtr func) { return func(f, s); }

int main(int argc, char **argv)
{
    calcFuncPtr calc = NULL;
    int a=0, b=0, result=0;
    char op=0;

	while(1) {

        scanf("%d %c %d", &a, &op, &b);

        if (a==0 || b==0 || op==0) break;

        switch(op) {
            case '+': calc = plus; break;
            case '-': calc = minu; break;
            case '*': calc = mult; break;
            case '/': calc = divi; break;
        }

        result = calculator(a, b, calc);
        printf("result ; %d\n", result);
    }

    return 0;
}

