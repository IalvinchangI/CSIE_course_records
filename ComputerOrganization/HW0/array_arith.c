#include <stdint.h>
#include <stdio.h>

int main()
{
    int32_t a[10] = {0}, b[10] = {0}, c[10] = {0};
    int32_t i, arr_size = 10;
    FILE *input = fopen("../testcases/input/2.txt", "r");
    for (i = 0; i < arr_size; i++)
        fscanf(input, "%d", &a[i]);
    for (i = 0; i < arr_size; i++)
        fscanf(input, "%d", &b[i]);
    fclose(input);
    int32_t *p_a = a;
    int32_t *p_b = b;
    int32_t *p_c = c;
    /* Original C code segment
    for (int32_t i = 0; i < arr_size; i++){
    *p_c++ = *p_a++ - *p_b++;
    }
    */
    __asm__ volatile(
		    "li t0, 0\n\t"  // t0 = i
		    "loop:\n\t"
		        "bge t0, %[arr_size], not_loop\n\t"
			"lw t1, 0(%[p_a])\n\t"  // t1 = p_a[i]
			"lw t2, 0(%[p_b])\n\t"  // t2 = p_b[i]
			"sub t3, t1, t2\n\t"  // t3 = t1 - t2
			"sw t3, 0(%[p_c])\n\t"  // p_c[i] = t3
			// next
			"addi t0, t0, 1\n\t"
			"addi %[p_a], %[p_a], 4\n\t"
			"addi %[p_b], %[p_b], 4\n\t"
			"addi %[p_c], %[p_c], 4\n\t"
			"j loop\n\t"
		    "not_loop:\n\t"
		     : 
		    [p_a] "+r"(p_a), [p_b] "+r"(p_b), [p_c] "+r"(p_c) : 
		    [arr_size] "r"(arr_size) : 
		    //"t0", "t1", "t2", "t3", "memory"
		    "memory"
    );

    p_c = c;
    for (int32_t i = 0; i < arr_size; i++)
        printf("%d ", *p_c++);
    printf("\n");
    return 0;
}
