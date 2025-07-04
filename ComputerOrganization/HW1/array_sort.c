#include <stdio.h>

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
    fclose(input);

    int *p_a = &arr[0];

    // array a bubble sort
    /* Original C code segment
    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i -1; j++) {
            if (*(p_a + j) > *(p_a + j + 1)) {
                int tmp = *(p_a + j);
                *(p_a + j) = *(p_a + j + 1);
                *(p_a + j + 1) = tmp;
            }
        }
    }
    */

    for (int i = 0; i < arr_size - 1; i++) {
        for (int j = 0; j < arr_size - i - 1; j++) {
            __asm__ volatile(
                // Your code
                "slli t3, %[j], 2\n\t"  // t3 = j * 4 = j << 2
                "add t2, %[p_a], t3\n\t"  // t2 = p_a + t3 (= current index)
                "lw t0, 0(t2)\n\t"  // t0 = p_a[j]
                "lw t1, 4(t2)\n\t"  // t1 = p_a[j + 1]
                "bge t1, t0, CONTINUE\n\t"  // t1 >= t0 --> continue
                    "sw t0, 4(t2)\n\t"
                    "sw t1, 0(t2)\n\t"
                "CONTINUE:"
                : 
		: 
       	        [j] "r"(j), [p_a] "r"(p_a) : 
		"t0", "t1", "t2", "t3", "memory"
            );
        }
    }
    p_a = &arr[0];
    for (int i = 0; i < arr_size; i++)
        printf("%d ", *p_a++);
    printf("\n");
    return 0;
}
