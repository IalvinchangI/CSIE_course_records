#include <stdio.h>

int arraySearch(int *p_a, int arr_size, int target)
{
    int result = -1;

    __asm__ volatile(
        // Your code
        "add t0, %[p_a], x0\n"  // t0 = pointer = p_a
        "addi t2, x0, -1\n"  // t2 = counter = -1
        "LOOP:"
            "bge t2, %[arr_size], END_LOOP\n"  // t2(counter) >= arr_size --> end
            "lw t1, 0(t0)\n"  // t1 = value = *t0
            "addi t0, t0, 4\n"  // t0(pointer) += 1
            "addi t2, t2, 1\n"  // t2(counter) += 1
            "bne t1, %[target], LOOP\n"  // t1 != targe --> loop again
            "add %[result], t2, x0\n"  // result = t2
        "END_LOOP:"
        :
        [result] "+r"(result) :
        [p_a] "r"(p_a), [arr_size] "r"(arr_size), [target] "r"(target) :
        "t0", "t1", "t2", "memory"
    );

    return result;
}

// Main function to test the implementation
int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int arr_size;
    fscanf(input, "%d", &arr_size);
    int arr[arr_size];

    // Read integers from input file into the array
    for (int i = 0; i < arr_size; i++) {
        int data;
        fscanf(input, "%d", &data);
        arr[i] = data;
    }
    int target;
    fscanf(input, "%d", &target);
    fclose(input);

    int *p_a = &arr[0];

    int index = arraySearch(p_a, arr_size, target);

    // Print the result
    printf("%d ", index);
    printf("\n");

    return 0;
}
