#include<stdio.h>
#include<stdlib.h>
#include"permutation_combination.h"

int main(void) {
    int *array, n = 0, r = 0;
    // combination
    scanf("%d %d", &n, &r);
    array = (int*) malloc(nCr(n, r) * sizeof(int) * r);
    clear_2Darray(nCr(n, r), r, array);
    combination(n, r, array);
    print_2Darray(nCr(n, r), r, array);
    free(array);

    // permutation
    scanf("%d %d", &n, &r);
    array = (int*) malloc(nPr(n, r) * sizeof(int) * r);
    clear_2Darray(nPr(n, r), r, array);
    permutation(n, r, array);
    print_2Darray(nPr(n, r), r, array);
    free(array);

    return 0;
}
