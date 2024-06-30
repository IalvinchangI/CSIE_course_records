#include<stdio.h>

int f(int i) {
    int array[3] = {0, 1, 2};
    if (i <= 2) {
        return array[i];
    }
    for (int j = 3; j <= i; j++) {
        int temp = array[0] + array[1] + array[2];
        array[0] = array[1];
        array[1] = array[2];
        array[2] = temp;
    }
    return array[2];
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