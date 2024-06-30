#include<stdio.h>
#include<time.h>
#include<stdlib.h>

int main(int argc, char* argv[]) {
    int n = atoi(argv[1]);
    int min = atoi(argv[2]);
    int max = atoi(argv[3]);
    srand((unsigned) time(NULL));
    for (int i = 0; i < n; i++) {
        printf("%d\n", rand() % (max - min + 1) + min);
    }
    return 0;
}
