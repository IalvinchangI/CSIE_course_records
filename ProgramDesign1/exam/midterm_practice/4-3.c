#include<stdio.h>

int f(int i, int compute[]) {
    if (i == 0) {
        return compute[0];
    }
    if (compute[i] == 0) {
        compute[i] = f(i - 1, compute) + f(i - 2, compute) + f(i - 3, compute);
    }
    return compute[i];
}

int main(void) {
    for (int i = 0; i < 8; i++) {
        int array[8] = {0, 1, 2};
        printf("%d\n", f(i, array));
    }
    return 0;
}