#include<stdio.h>

int main(int argc, char* argv[]) {
    for (int i = 0; i < 32;) {
        int IP = 0;
        for (int j = 128; j > 0; j = j >> 1) {
            if (argv[1][i] == '1') {
                IP |= j;
            }
            i++;
        }
        if (i != 32) {
            printf("%d.", IP);
        } else {
            printf("%d", IP);
        }
    }
    return 0;
}