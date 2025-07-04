# HW3

## Exercise 1. Implement FIFO Data Cache Replacement Policy
1. 用 `std::vector<std::queue<int>>` 紀錄資料抓入 cache 的順序，一個 set 用一個 queue，每個 queue 儲存的數字為 set 內的 index。
2. **`cache_sim_t::victimize`**：若 set 未滿，則尋找空的格子，紀錄 index。若 set 滿了，則在找到 set 對應的 queue 後，把最舊的 index pop 出來，並紀錄其 index。最後，將新值填入 index 的位置，並把格子的 index push 進對應的 queue。
3. **`fa_cache_sim_t::victimize`**：若 set 未滿，則把 index push 進對應的 queue。若 set 滿了，則在找到 set 對應的 queue 後，先把最舊的 index pop 出來，再將 index push 回去。最後，將新值填入 index 的位置。

## Exercise 2-1. Matrix Transpose Optimization
1. 將矩陣切成許多邊長為 `STEP` 的小正方形後，以小正方形為單位進行 transpose。
2. `STEP` 的大小為 cache 中一個 block 能存的 int 數（8）。

## Exercise 2-2. Matrix Multiplication Optimization
1. 將 `Output` 初始化為全 0 的陣列。
2. 用和 Exercise 2-1 相同的邏輯，把 `A`、`B` 矩陣切成許多邊長為 `STEP` 的小正方形，以小正方形為單位乘，把結果"加"入 `Output` 中對應的欄位。
3. `STEP` 的大小一樣為 cache 中一個 block 能存的 int 數（8）。

* **另一種設計方法**：
    1. 將 `B` transpose 後存入新的陣列 `B_`，方法和 Exercise 2-1 相同。
    2. 將 `A` 和 `B_` 的列相互進行相乘取總和後，存入 `Output` 中對應的欄位。
    ```c
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
    ```



## 測試
* 修改 `testbench_driver.c` 中的 `GOLDEN_RATIO_32`
