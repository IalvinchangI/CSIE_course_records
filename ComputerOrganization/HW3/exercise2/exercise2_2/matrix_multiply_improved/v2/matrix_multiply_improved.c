#include <stdlib.h>
void matrix_multiply(int *A, int *B, int *Output, int i, int k, int j) {
    const int STEP = 8;
    int* B_ = (int*) malloc(sizeof(int) * k * j);
    // x: B_, y: B_
    for (int y = 0; y < j; y += STEP) {
        for (int x = 0; x < k; x += STEP) {
            for (int y_ = y; y_ < (y + STEP) && y_ < j; y_++) {  // small y
                for (int x_ = x; x_ < (x + STEP) && x_ < k; x_++) {  // small x
                    B_[y_ * k + x_] = B[x_ * j + y_];
                }
            }
        }
    }

    // x: output(j), y: output(i)
    for (int y = 0; y < i; y++) {
        for (int x = 0; x < j; x++) {
            int sum = 0;
            for (int z = 0; z < k; z++) {
                sum += A[y * k + z] * B_[x * k + z];
            }
            Output[y * j + x] = sum;
        }
    }

    free(B_);
}
