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

struct LinkedList* find(struct LinkedList* head, char target[20]) {
    while (head != NULL) {  // find
        if (strcmp(head -> name, target) == 0) return head;  // find it
        if ((head -> next) != NULL) {
            struct LinkedList* node = find(head -> next, target);
            if (node != NULL) return node;  // find it
            // not in this branch
        }
        head = head -> back_head;
    }
    return NULL;
}