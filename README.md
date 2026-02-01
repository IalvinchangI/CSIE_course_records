# CSIE course records
資訊相關課程的作業紀錄

## 課程
* #### 程式設計一 `ProgramDesign1`

    內容：
    1. 考試 `exam`
    2. 作業 `Homework`
    3. 助教課實作 `TA_class`
    4. 練習 `Practice`

***

* #### 程式設計二 `ProgramDesign2`

    內容：
    1. 期末專案 `FinalProject`
    2. 作業 `Homework`

***

* #### 基於遊戲的機器學習入門 `Game_AI`

    作業：
    1. 打磚塊 `Arkanoid` 
        * 模型：KNN 
        * 模組：scikit-learn 
    2. 迷宮車 `Maze_Car`
        * 模型：Decision Tree 
        * 模組：scikit-learn 
    3. 魷魚對戰 `swimming-squid-battle`
        * 模型：Q-table 
        * 模組：numpy 
    4. 影像辨識 `Grapycal`
        * 模型：LeNet 
        * 模組：grapycal 
    5. 坦克對戰 `TankMan`
        * 模型：Q-table 
        * 模組：numpy 

***

* #### 資料結構 `DataStructure`

    內容：
    1. 作業 `Homework`
    2. 練習 `Practice`

***

* #### 資料科學導論 `IntroductionToDataScience`

    內容：
    1. 競賽 `Competition`
    2. 期末專案 `FinalProject`
    3. 作業 `Homework`

***

* #### 計算機組織 `ComputerOrganization`

    作業：
    1. 測試環境 `HW0`
    2. 練習 RISC-V 指令集 `HW1`
    3. 比較各實作方法下的效能 `HW2`
    4. 針對 cache 優化程式效能 `HW3`

***

* #### 資料視覺化與探索 `DataVisualizationAndExploration`

    報告：
    1. 各地區長照涵蓋率落差之觀察 `project1`
    2. Instagram Reels 流量分析 `project2`

***

* #### 量子計算導論 `IntroductionToQuantumComputation`

    期末專案：
    1. implement 2-qubit Grover’s algorithm `GroverAlgorithm.py`
    2. the 3-qubit quantum Fourier transform `FourierTransform.py`
    3. verify Bell inequality `BellInequality.py`
    
    程式庫：
    * 電路生成與執行的工具、結果計算的工具 `QuantumTool.py`
    * 和 IBMQ 網站有關的工具 `Load.py`
    * 輸出結果與電路圖的工具 `FormalOutput.py`
    * IBMQ API token `IBMQ_API_Token.py`

***

* #### 微算機原理與應用 `MicroprocessorPrinciplesAndApplications`

    內容：
    1. 期末專案 `FinalProject`
    2. 作業 `Homework`
    3. 用組合語言寫的工具的 library `BasicOperation`
        * 此 library 提供 16 位元的加減乘除以及位元運算（PIC18F4520 只支援 8 位元運算）
    4. 操作周邊元件或裝置的工具的 library `HardwareTools`
        * 此 library 將周邊元件或裝置的初始配置等包在 `*_Initialize` 的函式內，使用者只須呼叫它就能自動完成配置
        * 周邊元件或裝置的操作也包裝起來，方便使用
