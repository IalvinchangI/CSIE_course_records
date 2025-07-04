import subprocess
import os
import numpy
import re

working_directory = "/home/CompOrg/Desktop/HW/CO_StudentID_HW2"

test_exercise = "test_exercise2_1"
testcases_file = "arraymul_input.txt"
TIMES = 100
max_value = 100

total_score = 10.8
pattern = re.compile(f"Obtained/Total scores:\\s*(\\d+\\.\\d+)/")

list_pattern = re.compile(r"student output: (\[[\d., ]+\])\n")

# Run the compiled executable with arguments
try:
    error_count = 0
    for i in range(TIMES):
        array = numpy.random.uniform(0, max_value, size=512)
        test_case_string = ' '.join(map(lambda x: f"{x:.6f}", array))
        with open(testcases_file, 'w') as f:
            # Write the elements of the array
            f.write(test_case_string + "\n")
        print(f"Running {i}:")
        
        # Run the compiled executable with the arguments, capturing stdout and stderr
        for array_size in (4, 8, 16, 32, 64, 128):
            print(f"    {array_size = }")
            user_input = f"{array_size}\n"
            result = subprocess.run(["make", test_exercise], input=user_input, capture_output=True, text=True, check=True, cwd=working_directory)

            # Get and print standard output and error
            match = list_pattern.search(result.stdout)
            if match != None:
                answer_list = list(eval(match.group(1)))
                with open("arraymul_input.txt") as file:
                    string = file.readline()
                    float_list = list(map(float, string.split()))

                    delta_2 = list()
                    for j in range(0, array_size):
                        delta_2.append((round(float_list[j] * float_list[j + array_size] + 66, 6) - answer_list[j]) ** 2)
                        # delta_2 += (round(float_list[j] * float_list[j + array_size] + 66, 6) - answer_list[j]) ** 2
                print(f"    {i}-{array_size}: delta = {max(delta_2) ** 0.5}\t |\t value = {round(float_list[delta_2.index(max(delta_2))] * float_list[delta_2.index(max(delta_2)) + array_size] + 66, 6)}")
                #print(f"    {i}-{array_size}: delta = {(delta_2 / array_size) ** 0.5}")

            match = pattern.search(result.stdout)
            if match:
                test_score = float(match.group(1))
                if test_score != total_score:
                    print(result.stdout)
                    error_count += 1
                    # break
                else:
                    print(f"    {i}-{array_size}: ok ({test_score})")
            else:
                print(result.stdout)
                error_count += 1
                # break

            if result.stderr:
                print(f"Standard Error: {result.stderr}")

        # if error_count != 0:
        #     break

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
