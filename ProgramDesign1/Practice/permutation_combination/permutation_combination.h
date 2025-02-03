/*
 * Author : Alvin Chang
 * Created on Sat Nov 11 10:33:47 2023
 * permutaion & combination
 */

#include"permutation_combination.c"

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
