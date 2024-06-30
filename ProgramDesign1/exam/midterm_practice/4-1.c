#include<stdio.h>

int f(int i) {
    if (i == 2) {
        return 2;
    }
    if (i == 1) {
        return 1;
    }
    if (i == 0) {
        return 0;
    }
    return f(i - 1) + f(i - 2) + f(i - 3);
}

int main(void) {
    printf("%d\n", f(0));
    printf("%d\n", f(1));
    printf("%d\n", f(2));
    printf("%d\n", f(3));
    printf("%d\n", f(4));
    printf("%d\n", f(5));
    printf("%d\n", f(6));
    printf("%d\n", f(7));
    return 0;
}