import argparse
import subprocess
import numpy as np
import sys
import os
import time

current_seed = 0

try:
    import cupy as cp
    HAS_GPU = True

    xor_sum_kernel = cp.ReductionKernel(
        'T x',          # 輸入參數類型 (Template)
        'T y',          # 輸出參數類型
        'x',            # Map:讀取元素
        'a ^ b',        # Reduce: 兩兩 XOR
        'y = a',        # Post-Map: 輸出結果
        '0',            # Identity: XOR 的單位元素是 0
        'xor_sum'       # Kernel 名稱
    )
except ImportError:
    HAS_GPU = False

# 設定檔案名稱
INPUT_FILE = 'matrix_input.txt'

# 設定矩陣元素最大值
MATRIX_VALUE_MAX = 1000

# 請將此處改為你編譯好的 C 執行檔名稱
# Windows 可能需要加 .exe，例如 'hw6.exe'
C_EXECUTABLE = './hw6' 
if sys.platform.startswith('win'):
    C_EXECUTABLE = 'hw6.exe'

def generate_and_calc(n):
    """
    根據環境自動選擇 GPU 或 CPU 進行生成與計算
    最後回傳: (正確的XOR值, 矩陣A(numpy), 矩陣B(numpy))
    """
    print(f"[*] Initializing for N={n}...")

    # ============================
    # 策略 A: 使用 GPU (CuPy)
    # ============================
    if HAS_GPU:
        print(f"[*] 🚀 GPU Detected (CuPy)! Using GPU for calculation.")
        
        # 1. GPU 生成數據
        # 使用 int32 以節省記憶體並符合 C 的 int
        start_gen = time.perf_counter()
        A_gpu = cp.random.randint(0, MATRIX_VALUE_MAX, size=(n, n), dtype=cp.int32)
        B_gpu = cp.random.randint(0, MATRIX_VALUE_MAX, size=(n, n), dtype=cp.int32)
        
        # 2. GPU 計算乘法與 XOR (這裡會比 CPU 快非常多)
        start_calc = time.perf_counter()
        C_gpu = cp.matmul(A_gpu, B_gpu)
        expected_xor = int(xor_sum_kernel(C_gpu, axis=None))  # axis=None 表示對整個矩陣做歸約
        end_calc = time.perf_counter()
        
        print(f"    - GPU Matrix Mul & XOR time: {end_calc - start_calc:.4f}s")

        # 3. 將資料轉回 CPU 準備寫檔
        # 這是必要之惡，因為寫檔還是要靠 CPU 硬碟 I/O
        print("    - Transferring data from GPU to CPU...")
        A_cpu = cp.asnumpy(A_gpu)
        B_cpu = cp.asnumpy(B_gpu)

    # ============================
    # 策略 B: 使用 CPU (NumPy)
    # ============================
    else:
        print(f"[*] 🐢 No GPU found. Using CPU (NumPy). This might be slow for N=5000.")
        
        start_gen = time.perf_counter()
        A_cpu = np.random.randint(0, MATRIX_VALUE_MAX, size=(n, n), dtype=np.int32)
        B_cpu = np.random.randint(0, MATRIX_VALUE_MAX, size=(n, n), dtype=np.int32)
        
        start_calc = time.perf_counter()
        C_cpu = np.matmul(A_cpu, B_cpu)
        expected_xor = int(np.bitwise_xor.reduce(C_cpu.flatten()))
        end_calc = time.perf_counter()
        
        print(f"    - CPU Matrix Mul & XOR time: {end_calc - start_calc:.4f}s")

    return expected_xor, A_cpu, B_cpu

def write_to_file(n, A, B):
    """
    將資料寫入文字檔 (這是 N=5000 時的最大瓶頸)
    """
    print(f"[*] Writing data to {INPUT_FILE} (Text mode)...")
    t_start = time.perf_counter()
    
    with open(INPUT_FILE, 'w') as f:
        f.write(f"{n}\n")
        # 使用 fmt='%d' 確保寫入整數，減少轉換開銷
        np.savetxt(f, A, fmt='%d')
        np.savetxt(f, B, fmt='%d')
        
    t_end = time.perf_counter()
    print(f"    - File Write time: {t_end - t_start:.4f}s")

def run_c_program(expected_result, num_threads):
    if not os.path.exists(C_EXECUTABLE):
        print(f"[!] Error: Executable '{C_EXECUTABLE}' not found.")
        sys.exit(1)

    print(f"[*] Running C program: {C_EXECUTABLE} {num_threads} ...")
    
    try:
        # 啟動 C Process
        process = subprocess.Popen(
            [C_EXECUTABLE, str(num_threads)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )

        # 計時 (包含 C 的讀檔 + 運算 + 輸出)
        start_time = time.perf_counter()
        
        # 輸入檔名
        stdout_data, stderr_data = process.communicate(input=f"{INPUT_FILE}\n")
        
        end_time = time.perf_counter()
        elapsed_time = end_time - start_time
        print(f"[*] C execution time (Total): {elapsed_time:.4f}s")

        if process.returncode != 0:
            print(f"[!] C program failed (Exit code {process.returncode})")
            print(f"Stderr: {stderr_data}")
            sys.exit(1)

        raw_output = stdout_data.strip()
        if not raw_output:
            print("[!] Error: No output from C program.")
            return

        c_result = int(raw_output)

        if c_result == expected_result:
            print("\n" + "="*40)
            print("✅ SUCCESS: Results match!")
            print(f"   Answer: {c_result}")
            print(f"   Time taken: {elapsed_time:.4f}s")
            print("="*40)
        else:
            print("\n" + "="*40)
            print("❌ FAILURE: Results mismatch.")
            print(f"   Python (Expected): {expected_result}")
            print(f"   C Code (Actual):   {c_result}")
            print("="*40)

    except Exception as e:
        print(f"[!] Python execution error: {e}")

def main():
    parser = argparse.ArgumentParser(description='Test Matrix XOR with Threads')
    parser.add_argument('-n', type=int, required=True, help='Size of matrix')
    parser.add_argument('-t', '--threads', type=int, default=1, help='Number of threads for C program')
    parser.add_argument('-s', '--seed', type=int, default=42, help='Random seed for reproducibility')
    args = parser.parse_args()

    # set random seed
    global current_seed
    current_seed = args.seed
    np.random.seed(args.seed)
    if HAS_GPU:
        cp.random.seed(args.seed)
    print(f"[*] seed: {current_seed}")

    # 1. 計算正確答案 (自動選 GPU/CPU)
    expected_val, A, B = generate_and_calc(args.n)

    # 2. 寫入檔案 (這是最慢的一步)
    write_to_file(args.n, A, B)

    # 3. 執行 C 並驗證
    run_c_program(expected_val, args.threads)

if __name__ == "__main__":
    main()
