import argparse
import subprocess
import numpy as np
import sys
import os
import time
import math

# 設定 C++ 執行檔名稱
C_EXECUTABLE = './hw7'
if sys.platform.startswith('win'):
    C_EXECUTABLE = 'hw7.exe'

# 地圖座標最大值
MAP_SIZE = 1000

AUTO_CALCULATE_THRESHOLD = 20

def generate_cities(n: int, seed: int):
    print(f"[*] Generating {n} cities with seed {seed}...")
    np.random.seed(seed)
    cities = np.random.randint(0, MAP_SIZE + 1, size=(n, 2))
    return cities

def load_cities_from_file(filepath):
    print(f"[*] Reading cities from {filepath}...")
    try:
        with open(filepath, 'r') as f:
            line = f.readline()
            if not line: raise ValueError("File is empty")
            n = int(line.strip())
            cities = np.loadtxt(f, dtype=int)
            if cities.ndim == 1 and n == 1: cities = cities.reshape(1, 2)
            if len(cities) != n: print(f"[!] Warning: Header says N={n}, but found {len(cities)} lines.")
            return n, cities
    except Exception as e:
        print(f"[!] Error reading file: {e}")
        sys.exit(1)

def calc_dist_matrix(cities):
    n = len(cities)
    dist_matrix = np.zeros((n, n))
    for i in range(n):
        for j in range(n):
            if i != j:
                d = np.linalg.norm(cities[i] - cities[j])
                dist_matrix[i][j] = d
    return dist_matrix

def solve_tsp_exact(cities):
    n = len(cities)
    print(f"[*] Computing EXACT optimal solution for N={n} (Held-Karp)...")
    if n > AUTO_CALCULATE_THRESHOLD:
        print(f"[!] Warning: N is too large for exact solver (Limit N<={AUTO_CALCULATE_THRESHOLD}).")
        return None

    dist_matrix = calc_dist_matrix(cities)
    A = {(1 << i | 1, i): dist_matrix[0][i] for i in range(1, n)}

    for m in range(3, n + 1):
        if n > 15: print(f"    - Processing subsets of size {m}/{n}...")
        new_A = {}
        for (mask, j), cost in A.items():
            for k in range(1, n):
                if not (mask & (1 << k)): 
                    new_mask = mask | (1 << k)
                    new_cost = cost + dist_matrix[j][k]
                    if (new_mask, k) not in new_A or new_cost < new_A[(new_mask, k)]:
                        new_A[(new_mask, k)] = new_cost
        A = new_A

    min_dist = float('inf')
    full_mask = (1 << n) - 1
    for j in range(1, n):
        cost = A.get((full_mask, j), float('inf')) + dist_matrix[j][0]
        if cost < min_dist: min_dist = cost
            
    print(f"    - Optimal Distance: {min_dist:.4f}")
    return min_dist

def write_to_file(path: str, n: int, cities: np.ndarray, optimal_len=None):
    input_path = path if path.endswith('.in') else path + '.in'
    print(f"[*] Writing input to {input_path}...")
    with open(input_path, 'w') as f:
        f.write(f"{n}\n")
        np.savetxt(f, cities, fmt='%d')
    if optimal_len is not None:
        save_answer_file(input_path, optimal_len)

def save_answer_file(input_path, optimal_len):
    output_path = input_path.replace('.in', '.out')
    print(f"[*] Saving answer to {output_path}...")
    with open(output_path, 'w') as f:
        f.write(f"{optimal_len:.6f}\n")

def validate_solution(cities, path, reported_length):
    n = len(cities)
    print(f"\n=== Path Validation Report ===")

    # 1. 格式與數量檢查
    if len(path) != n:
        print(f"❌ Error: Path length is {len(path)}, expected {n}.")
        return False

    # 2. 唯一性檢查 (Set Check)
    path_set = set(path)
    if len(path_set) != n:
        print(f"❌ Error: Path is INVALID (not a permutation).")
        print(f"   Unique cities visited: {len(path_set)}/{n}")
        missing = set(range(n)) - path_set
        if len(missing) < 10:
            print(f"   Missing cities: {missing}")
        return False
    
    # 3. 重新計算距離
    recalc_dist = 0.0
    for i in range(n):
        u = path[i]
        v = path[(i + 1) % n] # 回到起點
        dist = np.linalg.norm(cities[u] - cities[v])
        recalc_dist += dist

    print(f"Path Structure  : ✅ Valid (Visits all {n} cities exactly once)")
    print(f"Reported Length : {reported_length:.4f}")
    print(f"Verified Length : {recalc_dist:.4f}")

    # 4. 誤差檢查
    diff = abs(reported_length - recalc_dist)
    if diff < 0.01:
        print("Distance Check  : ✅ Correct (Matches reported value)")
        return True
    else:
        print(f"Distance Check  : ⚠️ Mismatch (Diff: {diff:.4f})")
        print("   -> Tip: Check your C++ distance formula (sqrt vs round) or float precision.")
        return False

def run_c_program(input_file: str, cities: np.ndarray, expected_result=None):
    if not os.path.exists(C_EXECUTABLE):
        print(f"[!] Error: {C_EXECUTABLE} not found.")
        sys.exit(1)
    
    print(f"[*] Running C++ program: {C_EXECUTABLE}...")
    
    try:
        start_time = time.perf_counter()
        process = subprocess.Popen(
            [C_EXECUTABLE],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        stdout_data, stderr_data = process.communicate(input=f"{input_file}\n")
        end_time = time.perf_counter()
    except Exception as e:
        print(f"[!] Execution failed: {e}")
        return

    print(f"[*] C++ execution time: {end_time - start_time:.4f}s")
    if process.returncode != 0:
        print(f"[!] C++ crashed. Stderr:\n{stderr_data}")
        print(stdout_data)
        return

    output_lines = stdout_data.strip().split('\n')
    if not output_lines:
        print("[!] No output from C++ program.")
        return

    try:
        # 解析輸出
        # 假設格式:
        # Line 1: Length (float)
        # Line 2: Path (space separated integers)
        user_length = float(output_lines[0].strip())
        
        user_path = []
        if len(output_lines) > 1:
            # 讀取第二行並轉為 int list
            raw_path = output_lines[1].strip().split()
            user_path = [int(x) for x in raw_path]
        
        print(f"\n=== User Result ===")
        print(f"Path Length: {user_length:.4f}")
        
        # 執行路徑驗證
        if user_path:
            is_valid = validate_solution(cities, user_path, user_length)
            if not is_valid:
                print("\n❌ VALIDATION FAILED: The path output by C++ is invalid.")
                return # 路徑錯了，後面也不用比對最佳解了
        else:
            print("\n⚠️ Warning: No path output found (Line 2 is empty). Skipping path validation.")

        # 比對標準答案 (如果有)
        if expected_result is not None:
            evaluate_accuracy(user_length, expected_result)
            
    except ValueError as e:
        print(f"[!] Failed to parse output: {e}\nRaw output:\n{stdout_data}")

def evaluate_accuracy(user_len, optimal_len):
    print(f"\n=== Accuracy Report ===")
    print(f"Optimal Length : {optimal_len:.4f}")
    print(f"User Length    : {user_len:.4f}")
    
    if optimal_len == 0: return
    gap = (user_len - optimal_len) / optimal_len * 100.0
    print(f"Gap (Error Rate): {gap:.4f}%")
    
    if gap < 0.001: print("Result: 🏆 PERFECT!")
    elif gap < 1.0: print("Result: ✅ EXCELLENT (< 1%)")
    elif gap < 5.0: print("Result: 👌 GOOD (< 5%)")
    else: print("Result: ⚠️ Needs Improvement")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', type=int)
    parser.add_argument('-s', '--seed', type=int, default=42)
    parser.add_argument('--input', default='tsp_input.in')
    parser.add_argument('--use-existing', action='store_true')
    args = parser.parse_args()

    input_file = args.input if args.input.endswith('.in') else args.input + '.in'
    optimal_len = None
    cities = None

    if not args.use_existing:
        if args.n is None: parser.error("-n required")
        cities = generate_cities(args.n, args.seed)
        if args.n <= AUTO_CALCULATE_THRESHOLD: optimal_len = solve_tsp_exact(cities)
        write_to_file(input_file, args.n, cities, optimal_len)
    else:
        if not os.path.exists(input_file):
            print(f"[!] Error: {input_file} not found.")
            sys.exit(1)
        n_in, cities = load_cities_from_file(input_file)
        
        out_file = input_file.replace('.in', '.out')
        if os.path.exists(out_file):
            with open(out_file) as f: optimal_len = float(f.readline().strip())
        elif n_in <= AUTO_CALCULATE_THRESHOLD:
            print("[*] Auto-calculating ground truth...")
            optimal_len = solve_tsp_exact(cities)
            if optimal_len: save_answer_file(input_file, optimal_len)

    # 傳入 cities 以便進行路徑驗證
    run_c_program(input_file, cities, optimal_len)

if __name__ == "__main__":
    main()