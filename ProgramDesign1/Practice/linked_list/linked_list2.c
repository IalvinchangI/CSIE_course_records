#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>

// order list
typedef struct node {
    int value;
    struct node* next;
} list;


// =============================== method =============================== //

void insert(list**, list*);  // put list into list, order them
bool delete(list**, int);  // find data and delete it; retrun success or not

bool contain(const list*, int);  // find data; retrun TF



// =============================== tool =============================== //

void print_list(const list*);


int main(void) {
    list *head = NULL;

    // input & insert
    printf("Insert:\n");
    int total;
    scanf("%d", &total);
    for(int i = 0; i < total; i++){
        list *new = malloc(sizeof(list));
        scanf("%d", &(new -> value));
        insert(&head, new);
    }
    print_list(head);

    // input & delete
    printf("Delete:\n");
    scanf("%d", &total);
    for(int i = 0; i < total; i++){
        int num;
        scanf("%d", &num);
        printf("%d : %s\n", num, (delete(&head, num) ? "True" : "False"));
    }
    print_list(head);

    // input contain
    printf("Contain:\n");
    scanf("%d", &total);
    for(int i = 0; i < total; i++){
        int num;
        scanf("%d", &num);
        printf("%d : %s\n", num, (contain(head, num) ? "True" : "False"));
    }
    return 0;
}



// =============================== method =============================== //

void insert(list** head, list* node) {
    // put list into list, reset node, order them
    while (*head != NULL && ((*head) -> value) < (node -> value)) {  // find
        head = &((*head) -> next);
    }
    // insert
    node -> next = *head;
    *head = node;
}

bool delete(list** head, int value) {
    // find data and delete it; retrun success or not
    while (*head != NULL && ((*head) -> value) != value) {  // find
        if (((*head) -> value) > value) return false;
        head = &((*head) -> next);
    }
    if (*head == NULL) return false;

    // delete
    list *del = *head;
    *head = (*head) -> next;
    free(del);
    return true;
}

bool contain(const list *head, int value) {
    // find data; retrun TF
    while (head != NULL) {  // find
        if ((head -> value) == value) return true;
        if ((head -> value) > value) return false;
        head = head -> next;
    }
    return false;
}



// =============================== tool =============================== //

void print_list(const list *head) {
    while (head != NULL) {
        printf("%d -> ", head -> value);
        head = head -> next;
    }
    printf("NULL\n");
}