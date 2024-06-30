#include<stdlib.h>
#include<string.h>
#include<stdio.h>

struct LinkedList;

struct LinkedList {
    char name[20];
    int passwd_public[10][10][10][10];
    int ***passwd_private;
    struct LinkedList *back_head;
    struct LinkedList *next;
};

int update_public_target(int (*array)[10][10][10][10], int idx1, int idx2, int idx3, int idx4, int val) {
    (*array)[idx1][idx2][idx3][idx4] = val;
    return 0;
}
