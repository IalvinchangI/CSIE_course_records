#include <stdint.h>
#include <stdio.h>

int main()
{
    int32_t a, b;
    FILE *input = fopen("../testcases/input/1.txt", "r");
    fscanf(input, "%d %d", &a, &b);
    fclose(input);
    /* a = a - b */
    __asm__ volatile(
		    "sub %[a], %[a], %[b]\n\t" : 
		    [a] "+r"(a) : 
		    [b] "r"(b)
		    );
    printf("%d\n", a);
    return 0;
}
