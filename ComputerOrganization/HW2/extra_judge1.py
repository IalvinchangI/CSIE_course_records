import subprocess
import os
import numpy
import re

working_directory = "/home/CompOrg/Desktop/HW/CO_StudentID_HW2"

test_exercise = "test_exercise1"
testcases_file = "fft_input.txt"
TIMES = 100
max_value = 10


total_score = 24
pattern = re.compile(f"Obtained/Total scores:\\s*(\\d+\\.\\d+)/")

# Run the compiled executable with arguments
try:
    error_count = 0
    for i in range(TIMES):
        #array = [-max_value] * 16
        array = numpy.random.uniform(-max_value, max_value, size=16)
        test_case_string = ' '.join(map(lambda x: f"{x:.6f}", array))
        with open(testcases_file, 'w') as f:
            # Write the elements of the array
            f.write(test_case_string + "\n")
        print(f"Running {i}: {test_case_string}")
        
        # Run the compiled executable with the arguments, capturing stdout and stderr
        result = subprocess.run(["make", test_exercise], capture_output=True, text=True, check=True, cwd=working_directory)

        # Get and print standard output and error
        match = pattern.search(result.stdout)
        if match:
            test_score = float(match.group(1))
            if test_score != total_score:
                print(result.stdout)
                error_count += 1
            else:
                print(f"{i}: ok ({test_score})")
        else:
            print(result.stdout)
            error_count += 1

        if result.stderr:
            print(f"Standard Error: {result.stderr}")

        # wait
        # input()

except subprocess.CalledProcessError as e:
    print(f"Execution failed: {e}")
    print(f"Return code: {e.returncode}")
    print(f"Standard Error: {e.stderr}")
    error_count += 1
except FileNotFoundError:
    print(f"Executable {output_executable} not found.")
    error_count += 1
finally:
    print("No Error" if error_count == 0 else f"{error_count} Error")
