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

int cmp(const void *a, const void *b) {
    return *(char*)a - *(char*)b;
}

void sort_name(char target[20]) {
    qsort(target, strlen(target), sizeof(char), cmp);
}
