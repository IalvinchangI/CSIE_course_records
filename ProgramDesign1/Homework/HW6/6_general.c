#include<stdio.h>
#include<stdbool.h>

void Homework6_1(void);
void Homework6_2(void);
void Homework6_3(void);
void Homework6_4(void);

void array_x_setter(int [], int);
int scan_int(void);

int main(void) {
    int option = 0;

    scanf("%d", &option);
    switch (option) {
        case 1:
            Homework6_1();
            break;
        case 2:
            Homework6_2();
            break;
        case 3:
            Homework6_3();
            break;
        case 4:
            Homework6_4();
            break;
        default:
            break;
    }
    return 0;
}

void array_x_setter(int array[], int N) {
    // store N values in array
    for (int i = 0; i < N; scanf("%d", array + i), i++);
}

int scan_int(void) {
    int a = 0;
    scanf("%d", &a);
    return a;
}

void Homework6_1(void) {
    // reverse input number
    int N = scan_int();
    int a[200000] = {0};
    array_x_setter(a, N);
    for (int i = N - 1; i >= 0; (i != 0 ? printf("%d ", a[i]) : printf("%d", a[i])), i--);
}

void Homework6_2(void) {
    // find missing number
    int N = scan_int() - 1;
    int a[200000] = {0};
    bool in_a[200000] = {false};
    for (int i = 0; i < N; scanf("%d", a + i), in_a[a[i] - 1] = true, i++);
    for (int i = 0; i < N; i++) {
        if (!in_a[i]) {
            printf("%d", i + 1);
            return;
        }
    }
    // printf("Can\'t find missing number\n");
}

void Homework6_3(void) {
    // range queries of sum
    int N = scan_int();
    int a[100000] = {0};
    array_x_setter(a, N);

    // prefix sum
    long long int sum[100000] = {0};
    for (int i = 0; i < N; sum[i] = (i == 0) ? a[0] : ((long long) a[i] + sum[i - 1]), i++);

    // get range
    int Q = scan_int();
    long long int output[100000] = {0};
    for (int i = 0; i < Q; i++) {
        int start = scan_int() - 1;
        int end = scan_int() - 1;
        output[i] = sum[end] - ((start == 0) ? 0 : sum[start - 1]);
    }
    // print sum
    for (int i = 0; i < Q; (i == Q - 1) ? printf("%lld", output[i]) : printf("%lld\n", output[i]), i++);
}

void Homework6_4(void) {
    // 2D-array swaps
    int n = scan_int();  // y
    int m = scan_int();  // x
    int a[1000][1000] = {{0}};
    for (int y = 0; y < n; array_x_setter(a[y], m), y++);

    // swap (y1, x1) (y2, x2)
    for (int i = 0; i < 5; i++) {
        int y1 = scan_int() - 1, x1 = scan_int() - 1;
        int y2 = scan_int() - 1, x2 = scan_int() - 1;
        int temp = a[y1][x1];
        a[y1][x1] = a[y2][x2];
        a[y2][x2] = temp;
    }
    // print
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < m; x++) {
            (x != m - 1) ? printf("%d ", a[y][x]) : printf("%d", a[y][x]);
        }
        if (y != n - 1) {
            printf("\n");
        }
    }
}