#include<stdio.h>

int check_overflow(int a, int b) {
    if (a > 0 && b > 0) {
        if (a + b < 0) {
            return 0;  // overflow
        }
    }
    if (a < 0 && b < 0) {
        if (a + b >= 0) {
            return 0;  // overflow
        }
    }
    return 1;  // no overflow
}

int main(void) {
    printf("%d\n", check_overflow(2137483647, 147483647));
    printf("%d\n", check_overflow(2147483647, 0));
    printf("%d\n", check_overflow(-2147483647, -2));
    printf("%d\n", check_overflow(2147483642, 5));
    printf("%d\n", check_overflow(-2147483647, -1));
    printf("%d\n", check_overflow(-2147483648, -2147483648));
    return 0;
}