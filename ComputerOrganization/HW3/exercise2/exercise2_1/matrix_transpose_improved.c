void matrix_transpose(int n, int *dst, int *src) {
    const int STEP = 8;
    // x: dst, y: dst
    for (int x = 0; x < n; x += STEP) {
        for (int y = 0; y < n; y += STEP) {
            for (int x_ = x; x_ < (x + STEP) && x_ < n; x_++) {  // small x
                for (int y_ = y; y_ < (y + STEP) && y_ < n; y_++) {  // small y
                    dst[y_ * n + x_] = src[x_ * n + y_];
                }
            }
        }
    }
}
