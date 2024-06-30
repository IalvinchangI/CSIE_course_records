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

int check_index(int index) {
    return (index >= 10 || index < 0) ? 1 : 0;
}

int update_public(struct LinkedList* head, char target[20], int idx1, int idx2, int idx3, int idx4, int* val) {
    struct LinkedList* place = find(head, target);
    if (place == NULL) {  // failed
        return -1;
    }
    if (check_index(idx1) || check_index(idx2) || check_index(idx3) || check_index(idx4)) {
        return -2;  // out of range
    }
    // success
    int temp = (place -> passwd_public)[idx1][idx2][idx3][idx4];
    (place -> passwd_public)[idx1][idx2][idx3][idx4] = *val;
    *val += temp;
    return 0;
}
