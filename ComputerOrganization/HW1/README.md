# HW1

## Exercise 1. Array Bubble Sort
1. 使用 `slli` 把 `j` 乘 4 倍
2. 把陣列中的元素對調時，直接把已存入 register 的元素的值存進另一個元素的記憶體內，不透過中繼的變數來達成
    ```
    sw t0, 4(t2)
    sw t1, 0(t2)
    ```

## Exercise 2. Array Search
1. 在迴圈（`LOOP`）的尾端檢查元素是否和 target 相同，若不相同則 branch 回迴圈的開頭；若相同則直接往下執行下面這行程式
    ```
    add %[result], t2, x0
    ```
2. 上面這行程式在 `END_LOOP` 上方，所以當迴圈跑完所有元素時會直接跳過這行，直接結束。也就是說，只有在找到目標元素時，`result` 的值才會被更改

## Exercise 3. Linked-List Merge Sort
* **WARNING**：**`RISC-V64 的位址大小為 64bits`**
1. **`splitList`**：
    1. 不計算 linked list 長度
    2. 執行迴圈時，紀錄當前的節點（`cur`）和第一段 linked list 的結尾（`firstTail`）
    3. 每次迴圈中，`cur` 往前跑 2 個節點，`firstTail` 往前跑 1 個
    4. 最後把 `firstTail->next` 的值設為 NULL，標示第一段 linked list 的結尾
    * 演算法：
        ```c
        Node *cur = head->next;
        Node *firstTail = head;
        while (1) {
            cur = cur->next;
            if (cur == NULL) {
                break;
            }
            firstTail = firstTail->next;

            cur = cur->next;
            if (cur == NULL) {
                break;
            }
        } 
        *firstHalf = head;
        *secondHalf = firstTail->next;
        firstTail->next = NULL;
        ```
2. **`mergeSortedLists`**：
    1. 就算是用 assembly，不能存取的記憶體位置還是不能存取，所以在存取節點前要先判斷指向的位置是不是 NULL
    2. 每次檢查 `a->next` 或 `b->next` 時，如果其值為 NULL，那麼直接跳到 `IF3` 或 `ELSE3`，減少需要執行的程式
    * 演算法：（邏輯大致相似）
        ```c
        if (a->data < b->data) {
            result = a;
            tail = a;
            a = a->next;
        }
        else {
            result = b;
            tail = b;
            b = b->next;
        }
        while (a != NULL && b != NULL) {
            if (a->data < b->data) {
                tail->next = a;
                tail = a;
                a = a->next;
            }
            else {
                tail->next = b;
                tail = b;
                b = b->next;
            }
        }
        if (a == NULL) {
            tail->next = b;
        }
        else {
            tail->next = a;
        }
        ```

## 測試
* 先用 python 隨機生成測資，再使用 `subprocess.run()` 來執行，最後檢查輸出結果
