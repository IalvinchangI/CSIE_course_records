import subprocess
import os
import numpy

working_directory = "/home/CompOrg/Desktop/HW/HW1/CO_StudentID_HW1"

# Define the C source file and the output executable name
c_file = 'linked_list_sort.c'        # Your C source file
output_executable = '3'  # The name of the compiled executable
testcases_file = "extra_testcase.txt"
TIMES = 100
max_value = 101

# Step 1: Compile the C source file into an executable
try:
    print(f"Compiling {c_file}...")

    # Run gcc to compile the C file into an executable
    compile_command = ["riscv64-unknown-linux-gnu-gcc", "-static", c_file, '-o', output_executable]
    subprocess.run(compile_command, check=True, cwd=working_directory)

    print(f"Compilation successful. Executable created: {output_executable}")
except subprocess.CalledProcessError as e:
    print(f"Compilation failed: {e}")
    exit(1)

# Step 2: Run the compiled executable with arguments
try:
    error_count = 0
    for i in range(TIMES):
        length = numpy.random.randint(1, max_value, size=1)
        array = numpy.random.choice(range(1, max_value), size=length, replace=False)
        with open(testcases_file, 'w') as f:
            # Write the length of the array
            f.write(f"{length[0]}\n")
            
            # Write the elements of the array
            f.write(' '.join(map(str, array)) + "\n")
        print(f"Running {i}:")
        
        # Run the compiled executable with the arguments, capturing stdout and stderr
        result = subprocess.run(["spike", "--isa=RV64GCV", os.path.expandvars("$RISCV/riscv64-unknown-linux-gnu/bin/pk"), output_executable, testcases_file], capture_output=True, text=True, check=True, cwd=working_directory)

        # Get and print standard output and error
        expected_result = ' '.join(map(str, sorted(array))) + " \n"
        print(f"expected result: {expected_result}", end="")
        print(f"Standard Output: {result.stdout}")
        if expected_result != result.stdout:
            print("ERROR")
            error_count += 1

        if result.stderr:
            print(f"Standard Error: {result.stderr}")

except subprocess.CalledProcessError as e:
    print(f"Execution failed: {e}")
    print(f"Return code: {e.returncode}")
    print(f"Standard Error: {e.stderr}")
except FileNotFoundError:
    print(f"Executable {output_executable} not found.")
finally:
    print(f"{error_count = }")
