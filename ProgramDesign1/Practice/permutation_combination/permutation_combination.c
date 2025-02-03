/*
 * Author : Alvin Chang
 * Created on Sat Nov 11 10:33:47 2023
 * permutaion & combination
 */

#include<stdlib.h>

// output = malloc
static int* __permutation(int, int, int*, int, int*);  // nPr recursion
void permutation(int, int, int*);  // nPr
static int* __combination(int, int, int*, int);  // nCr recursion
void combination(int, int, int*);  // nCr

// calculate
int factorial(int);  // !
int nPr(int, int);  // nPr
int nCr(int, int);  // nCr

// array = malloc
static int* index2address(int , int , int *, int);  // for 2D malloc
void clear_1Darray(int, int*);  // fill 0  // for 1D malloc
void clear_2Darray(int, int, int*);  // fill 0  // for 2D malloc
void print_2Darray(int, int, int*);  // for 2D malloc


static int* __permutation(int n, int r, int *output, int inner_length, int *used_TF) {
    // nPr recursion  // output = malloc
    static int outer_index = 0;
    for (int number = 0; number < n; number++) {
        if (*(used_TF + number)) {  // number has used
            continue;
        }
        *index2address(outer_index, r - 1, output, inner_length) = number + 1;
        if (r == 1) {  // stop recursion
            outer_index++;
            continue;
        }
        // recursion
        *(used_TF + number) = 1;
        __permutation(n, r - 1, output, inner_length, used_TF);  // choose one number from 1 ~ n
        *(used_TF + number) = 0;
    }
    return &outer_index;
}

void permutation(int n, int r, int *output) {
    // nPr  // output = malloc
    // find all permutation
    int *used_TF = (int*) malloc(sizeof(int) * n);
    clear_1Darray(n, used_TF);
    int *outer_index = __permutation(n, r, output, r, used_TF);

    // fill output
    int copy_index = 0;
    for (int index = 0; index < *outer_index; index++) {
        for (int i = r - 1; i > 0; i--) {
            if (*index2address(index, i, output, r) != 0) {
                copy_index = index;
                continue;
            }
            *index2address(index, i, output, r) = *index2address(copy_index, i, output, r);
        }
    }
    // init outer_index
    *outer_index = 0;
    free(used_TF);
}

static int* __combination(int n, int r, int *output, int inner_length) {
    // nCr recursion  // output = malloc
    static int outer_index = 0;
    for (int new_n = r - 1; new_n < n; new_n++) {
        // new_n + 1 is equal to the number we want to store
        *index2address(outer_index, r - 1, output, inner_length) = new_n + 1;
        if (r == 1) {  // stop recursion
            outer_index++;
            continue;
        }
        // recursion
        __combination(new_n, r - 1, output, inner_length);
    }
    return &outer_index;
}

void combination(int n, int r, int *output) {
    // nCr  // output = malloc
    // find all combination
    int *outer_index = __combination(n, r, output, r);

    // fill output
    int copy_index = 0;
    for (int index = 0; index < *outer_index; index++) {
        for (int i = r - 1; i > 0; i--) {
            if (*index2address(index, i, output, r) != 0) {
                copy_index = index;
                continue;
            }
            *index2address(index, i, output, r) = *index2address(copy_index, i, output, r);
        }
    }
    // init outer_index
    *outer_index = 0;
}


int factorial(int n) {
    int sum = 1;
    for (int i = 1; i <= n; i++) {
        sum *= i;
    }
    return sum;
}

int nPr(int n, int r) {
    return factorial(n) / factorial(n - r);
}

int nCr(int n, int r) {
    return factorial(n) / factorial(n - r) / factorial(r);
}


static int* index2address(int i, int j, int *array2D, int inner_length) {
    // input index, array; output address  // for 2D malloc
    return array2D + i * inner_length + j;
}

void clear_1Darray(int i, int *array) {
    // fill 0
    for (int x = 0; x < i; x++) {
        *(array + x) = 0;
    }
}

void clear_2Darray(int i, int j, int *array) {
    // fill 0
    for (int y = 0; y < i; y++) {
        for (int x = 0; x < j; x++) {
            *(array + x + y * j) = 0;
        }
    }
}

void print_2Darray(int i, int j, int *array) {
    printf("==============================================\n");
    for (int y = 0; y < i; y++) {
        for (int x = 0; x < j; x++) {
            printf("%d ", *(array + x + y * j));
        }
        printf("\n");
    }
    printf("==============================================\n");
}