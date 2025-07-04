void matrix_multiply(int *A, int *B, int *Output, int i, int k, int j) {
    // init
    for (int y = 0; y < i; y++) {
        for (int x = 0; x < j; x++) {
            Output[y * j + x] = 0;
        }
    }

    // multiply
    const int STEP = 8;
    for (int y_ = 0; y_ < i; y_ += STEP) {

        // in B
        for (int m_ = 0; m_ < k; m_ += STEP) {
            for (int x_ = 0; x_ < j; x_ += STEP) {

                // small matrix
                for (int y = y_; y < (y_ + STEP) && y < i; y++) {
                    for (int m = m_; m < (m_ + STEP) && m < k; m++) {
                        for (int x = x_; x < (x_ + STEP) && x < j; x++) {
                            Output[y * j + x] += A[y * k + m] * B[m * j + x];
                        }
                    }
                }
            }
        }
    }
}
