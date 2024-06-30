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

int update_name(struct LinkedList* head,char target[20],char modify[20]) {
    struct LinkedList* place = find(head, target);
    if (place == NULL) {  // failed
        return -1;
    }
    strcpy(place -> name, modify);
    return 0;
}
