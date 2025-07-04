# HW2

## Exercise 1. Fast Fourier Transform Calculation
1. **complex_mul**：先將各種實部、虛部組合的積求出並存入 `f1` ~ `f4`，再將他們相減或相加的結果存入 `result`
2. **log2**：計算 `N` 右移幾次會變成 0
3. **pi**：
    1. `f1` = 1.0f
    2. 一次迴圈中，會做一次加的一次減的，省去判斷何時使用加或減
4. **bit_reverse**：
    1. `t0`：讀取 bit 用的遮罩，每讀完一個 bit 就會向右位移 1 格
    2. `t1`：暫存 `t0` 讀出的值、將值寫入 `t2`（寫入時，永遠寫在第 `m` 個 bit）
    3. `t2`：儲存結果。`t0` 每讀完一個 bit，`t2` 就會向右位移 1 格，如此一來，就不用計算寫入的位子，寫入的值會隨著一次次的迴圈慢慢移到對的位置

## Exercise 2-1. Array Multiplication without V Extension
1. 一次計算 1 個值
2. `arr_size`：一開始會被轉換成 `arr_size` * 4 + `h` ，用以作為跳出迴圈的依據
3. `h`、`x`、`y` 每次都加 4

## Exercise 2-2. Array Multiplication with V Extension
1. 一次計算 $\frac{vlen}{32}$ 個值
2. 每次迴圈都重新計算一次 vl，並將結果存在 `t0`
3. `h`、`x`、`y` 每次都加 vl
4. `arr_size`：每次都減 vl

## Exercise 3-1. Single Floating-point Multiplication
1. `result` 會被初始化為 1.0
2. `arr_size`：一開始會被轉換成 `arr_size` * 4 + `h` ，用以作為跳出迴圈的依據
3. `h`、`x` 每次都加 4

## Exercise 3-2. Double Floating-point Multiplication
1. `result` 會被初始化為 1.0
2. `arr_size`：一開始會被轉換成 `arr_size` * 8 + `h` ，用以作為跳出迴圈的依據
3. `h`、`x` 每次都加 8



## 測試
* 先用 python 隨機生成測資，再使用 `subprocess.run()` 執行作業中附的 judge 系統

## Issue
* Exercise1 的 `complex_mul` 在編譯時會發生使用相同 register 的問題。因為 `result` 使用了 `f1` ~ `f4` 中的其中一個 register，導致 `result` 的值會在計算過程中被覆寫，產生預料外的結果。
* Exercise2 的 judge 系統的容錯範圍過小，會把小數點後大概 3 位的誤差當成錯誤答案。float 本身只有 23 bit 能存數值，換算成 10 進位為 8,400,000 左右，也就是說大概有 7 個位數是準的，所以當輸出數字是 4 位數時，小數點後第 3 位開始就有機會出現誤差。另外，因為輸入最大值是 3 位數且小數點後有 6 位，所以存入記憶體後本身就有誤差了，後續計算只會產生更大的誤差。
