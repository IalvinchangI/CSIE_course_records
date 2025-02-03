#include<stdio.h>

int hanoi(int, int, int, int);
void print_move(int, int, int);

int main(void) {
    int N = 0;
    scanf("%d", &N);
    printf("step: %d\n", hanoi(N, N, 1, 3));
    return 0;
}

int hanoi(int move_count, int bottom_number, int from, int to) {
    if (move_count == 1) {  // print & stop recursion
        print_move(bottom_number, from, to);
        return 1;
    }
    // recursion
    return hanoi(move_count - 1, bottom_number - 1, from, from ^ to) + hanoi(1, bottom_number, from, to) + hanoi(move_count - 1, bottom_number - 1, from ^ to, to);
}

void print_move(int number, int from, int to) {
    printf("move %d from %c to %c\n", number, from + 64, to + 64);
}