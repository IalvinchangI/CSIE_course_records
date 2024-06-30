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

int insert(struct LinkedList* head, char target[20], struct LinkedList* insert_people) {
    struct LinkedList* place = find(head, target);
    if (place == NULL) {  // failed
        return -1;
    }
    if ((place -> back_head) == NULL) {  // success & no one after him
        place -> back_head = insert_people;
        return 0;
    }
    struct LinkedList** next = &(place -> back_head);
    while (*next != NULL) {  // find right
        next = &((*next) -> next);
    }
    *next = insert_people;
    return 0;  // success
}