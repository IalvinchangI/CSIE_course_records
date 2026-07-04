# 六階段漸進式開發與測試計畫

### 階段一：基礎資料結構建置 (Data Models)
**目標**：建立最底層不會變動的資料容器，包含檔名記憶，並確保其支援 Pickle 序列化。

* **實作步驟 (`models.py`)**：
    1.  實作 `FeatureLine`，僅接收 `start_point` 與 `end_point`。
    2.  實作 `MorphImage`，初始化傳入 `image` (numpy array) 與 **`original_filename` (str)**，並建立 `add_line`, `get_lines`, `clear_lines` 等管理方法。
* **對應測試 (Unit Test)**：
    * ✅ **T1-1 (新增測試)**：實例化 `MorphImage`，連續 `add_line` 三次，斷言 `len(get_lines()) == 3`。
    * ✅ **T1-2 (序列化與檔名測試)**：將帶有特徵線與 `original_filename='test.jpg'` 的 `MorphImage` 透過 `pickle.dumps` 序列化，再 `pickle.loads` 反序列化。斷言解開後的 image 矩陣數值相同、特徵線座標未遺失，且 `original_filename` 依然為 `'test.jpg'`。

### 階段二：獨立變形引擎開發 (MorphEngine Core)
**目標**：驗證 Beier-Neely 核心數學公式、邊界保護，以及 **CuPy 硬體加速切換** 是否正確。

* **實作步驟 (`morpher.py`)**：
    1.  定義類別常數 $a=0.001, b=2.0, p=0.5$。
    2.  **實作模組偵測**：嘗試 `import cupy`，若成功則設定內部旗標啟用 GPU 加速，否則降級使用 NumPy。
    3.  實作 `_interpolate_lines` (根據 $t$ 內插兩組特徵線)。
    4.  實作最核心的 `_compute_warp`：利用 meshgrid 產生座標，套用向量化公式，最後套用 `cv2.remap(..., borderMode=cv2.BORDER_REPLICATE)`。
    5.  實作 `process`，串接上述兩者。
* **對應測試 (Unit/Script Test)**：
    * ✅ **T2-1 (內插精準度)**：輸入線段 $A(0,0)\to(10,10)$ 與 $B(10,10)\to(20,20)$，設定 $t=0.5$，斷言輸出的過渡線段為 $(5,5)\to(15,15)$。
    * ✅ **T2-2 (加速器切換測試)**：若環境有 CuPy，斷言引擎的 `process` 回傳的依然是標準的 `numpy.ndarray` (引擎內部需做好 `.get()` 轉換，不可把 cupy array 洩漏給 UI)。
    * ✅ **T2-3 (單線變形與邊界不崩潰)**：傳入極端的特徵線（會將像素推到畫面外），呼叫 `_compute_warp` 並用 `cv2.imshow` 視覺確認正方形被正確拉伸且邊緣有被延伸 (BORDER_REPLICATE)，無 `IndexError`。

### 階段三：互動式標記畫布 (ImageEditor)
**目標**：讓使用者能夠順暢地在圖片上畫線，並以「箭頭」提供明確的方向性視覺回饋。

* **實作步驟 (`editor.py`)**：
    1.  建立 `ImageEditor`，綁定 `MorphImage`。
    2.  實作 `_mouse_callback` 處理點擊、拖曳、放開事件。
    3.  實作 `_draw_overlay`：**使用 `cv2.arrowedLine` (而非普通的 line)**，動態計算顏色 `color = palette[index % len]` 並使用 `cv2.putText` 標上序號。
* **對應測試 (Manual UI Test)**：
    * ✅ **T3-1 (箭頭方向與序號驗證)**：開啟畫布，由左上往右下畫一條線。確認終點處有「箭頭」指示方向，旁邊標示序號 `0`。
    * ✅ **T3-2 (即時預覽測試)**：滑鼠按下並拖曳時，畫面上必須能即時看到跟著游標移動的箭頭虛線/實線。

### 階段四：主視窗與多執行緒防抖 (GUI & Multithreading)
**目標**：建立 Tkinter 介面，打通 Producer-Consumer 執行緒架構。

* **實作步驟 (`gui.py`)**：
    1.  用 Tkinter 刻出 2x2 Grid、Alpha 拉桿 (**強制限制 `resolution=0.01`**)、ROI 拉桿與 Checkbox。
    2.  建立 `input_queue` 與 `output_queue`。
    3.  實作 `on_slider_changed` 打包參數至 Input Queue。
    4.  實作 `worker_loop` 的 **LIFO 清空 Queue 防抖邏輯** (暫以 `time.sleep(0.2)` 假裝運算)。
    5.  實作 `update_ui_loop` 輪詢更新。
* **對應測試 (Concurrency Test)**：
    * ✅ **T4-1 (解析度與防抖測試)**：推動 Alpha 拉桿，斷言其數值變化單位精確為 `0.01`。左右狂拉時，視窗絕對不可卡死；停止拉動後，畫面只會更新最後一次的參數結果，不處理堆積的舊任務。

### 階段五：系統整合與 ROI 拼貼 (Integration & Splicing)
**目標**：把真實的 `MorphEngine` 接進 Worker Thread，並實作進階的遮罩羽化拼貼。

* **實作步驟 (`gui.py` 的 `worker_loop`)**：
    1.  拔除假運算，接上 `MorphEngine.process(img_A, img_B, alpha)`。
    2.  實作 ROI 邏輯：產生 Alpha Mask，使用 `cv2.GaussianBlur` 羽化，套用 `Result = B*Mask + A*(1-Mask)`，並 `np.copy` 傳給 UI。
* **對應測試 (Visual/Integration Test)**：
    * ✅ **T5-1 (ROI 羽化拼貼測試)**：Alpha 設為 0.5，開啟 ROI 將範圍縮小到中央。斷言 Slot 3 中央為 `Warped_B`，外圍為 `Warped_A`，交界處為平滑過渡無鋸齒。

### 階段六：系統持久化與打包 (I/O & Packaging)
**目標**：處理指定槽位 CLI、動態載入、資料夾選擇儲存，並產生執行檔。

* **實作步驟 (`main.py` & `gui.py`)**：
    1.  實作 `main.py` 的 CLI `argparse`：加入如 `--slot1`, `--slot2`, `--slot3` 的參數來對應槽位載入圖片或 pickle 檔。
    2.  實作「載入按鈕」：若槽位為空則開啟 `filedialog.askopenfilename`；若已有圖片則直接開啟 `ImageEditor` 允許繼續添加線條。
    3.  實作「儲存按鈕」：開啟 `filedialog.askdirectory` 取得資料夾，並以 `original_filename` + `.pkl` 作為預設檔名進行 `pickle.dump`。
    4.  執行 `PyInstaller` 進行打包。
* **對應測試 (E2E Test)**：
    * ✅ **T6-1 (動態儲存與 CLI 還原測試)**：點擊載入圖 1，畫 3 條線。點擊儲存，選擇桌面，確認檔名預設為原檔名 `.pkl`。關閉程式後，透過 CLI `python main.py --slot2 桌面/原檔名.pkl` 啟動，斷言這張圖出現在「圖 2」的槽位，且箭頭線條完好如初。
    * ✅ **T6-2 (跨平台打包)**：執行產出的執行檔，確認無 `ModuleNotFoundError`。
