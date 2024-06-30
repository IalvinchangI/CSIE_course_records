#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<stdbool.h>

void Homework6_1(void);
void Homework6_2(void);
void Homework6_3(void);
void Homework6_4(void);

void array_x_setter(int [], int, int);
int random_int(int, int);

int main(void) {
    int option = 0;

    srand((unsigned) time(NULL));
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
            while (1) {
                int a = 0, b = 0;
                scanf("%d %d", &a, &b);
                printf("%d\n", random_int(a, b));
            }
            break;
    }
    return 0;
}

void array_x_setter(int array[], int N, int max) {
    // store N values in array (max = ?, min = 1)
    for (int i = 0; i < N; i++) {
        array[i] = random_int(1, max);
        (i != N - 1) ? printf("%d ", array[i]) : printf("%d\n", array[i]);  // <input> array number
    }
}

int random_int(int min, int max) {
    // generate random int
    return (rand() % (max - min + 1)) + min;
}

void Homework6_1(void) {
    // reverse input number
    int N = random_int(5000, 200000);  // 5000 ~ 2 * 10^5
    printf("%d\n", N);  // <input> N
    int a[200000] = {0};
    array_x_setter(a, N, 1000000000);  // 1 ~ 10^9  // <input> array number
    for (int i = N - 1; i >= 0; (i != 0 ? printf("%d ", a[i]) : printf("%d", a[i])), i--);  // <output>
}

void Homework6_2(void) {
    // find missing number
    int N = random_int(5000, 200000);  // 5000 ~ 2 * 10^5
    printf("%d\n", N);  // <input> N
    int a[200000] = {0};
    bool in_a[200000] = {false};
    for (int i = 0; i < N - 1; i++) {
        while (1) {  // prevent store same numbers
            a[i] = random_int(1, N);  // 1 ~ N
            if (!in_a[a[i] - 1]) {
                in_a[a[i] - 1] = true;
                printf("%d", a[i]);  // <input> array number
                break;
            }
        }
        if (i == N - 1) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
    for (int i = 0; i < N; i++) {  // <output>
        if (!in_a[i]) {
            printf("%d", i + 1);
            return;
        }
    }
    // printf("Can\'t find missing number\n");
}

void Homework6_3(void) {
    // range queries of sum
    int N = random_int(50000, 100000);  // 50000 ~ 10^5
    printf("%d\n", N);  // <input> N
    int a[100000] = {0};
    array_x_setter(a, N, 1000000000);  // 1 ~ 10^9  // <input> array number

    // prefix sum
    long long int sum[100000] = {0};
    for (int i = 0; i < N; sum[i] = (i == 0) ? a[0] : ((long long) a[i] + sum[i - 1]), i++);

    // get range
    int Q = random_int(50000, 100000);  // 50000 ~ 10^5
    printf("%d\n", Q);  // <input> Q
    long long int output[100000] = {0};
    for (int i = 0; i < Q; i++) {
        int start = random_int(1, N);  // 1 ~ N
        int end = random_int(1, N);  // 1 ~ N
        if (start > end) {
            int temp = start;
            start = end;
            end = temp;
        }
        printf("%d %d\n", start, end);  // <input> start end
        start--;
        end--;
        output[i] = sum[end] - ((start == 0) ? 0 : sum[start - 1]);
    }
    // print sum
    for (int i = 0; i < Q; (i == Q - 1) ? printf("%lld", output[i]) : printf("%lld\n", output[i]), i++);  // <output>
}

void Homework6_4(void) {
    // 2D-array swaps
    int n = random_int(500, 1000);  // y  // 500 ~ 1000
    int m = random_int(500, 1000);  // x  // 500 ~ 1000
    printf("%d %d\n", n, m);  // <input> n m
    int a[1000][1000] = {{0}};
    for (int y = 0; y < n; array_x_setter(a[y], m, 1000), y++);  // 1 ~ 1000  // <input> array number 2D

    // swap (y1, x1) (y2, x2)
    for (int i = 0; i < 5; i++) {
        int y1 = random_int(1, n);  // 1 ~ n
        int x1 = random_int(1, m);  // 1 ~ m
        int y2 = random_int(1, n);  // 1 ~ n
        int x2 = random_int(1, m);  // 1 ~ m
        printf("%d %d %d %d\n", y1, x1, y2, x2);  // <input> swap-y1 swap-x1 swap-y2 swap-x2
        y1--;
        x1--;
        y2--;
        x2--;
        int temp = a[y1][x1];
        a[y1][x1] = a[y2][x2];
        a[y2][x2] = temp;
    }
    // print  // <output>
    for (int y = 0; y < n; y++) {
        for (int x = 0; x < m; x++) {
            (x != m - 1) ? printf("%d ", a[y][x]) : printf("%d", a[y][x]);
        }
        if (y != n - 1) {
            printf("\n");
        }
    }
}