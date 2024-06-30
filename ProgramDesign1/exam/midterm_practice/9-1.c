#include<stdio.h>

int main(int argc, char* argv[]) {
    int index = 0;
    for (int i = 0; i < 4; i++) {
        int IP = 0;
        while (1) {
            char c = argv[1][index];
            index++;
            if (c == '.' || c == '\0') {
                break;
            }
            IP *= 10;
            IP += c - '0';
        }
        for (int j = 128; j > 0; j = j >> 1) {
            if (j & IP) {
                printf("1");
            } else {
                printf("0");
            }
        }
        // printf(" ");
    }
    return 0;
}