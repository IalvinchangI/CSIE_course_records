#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node* prev;
    struct Node* next;
};

void insertNode(struct Node** head, int newData) {
    struct Node *new = malloc(sizeof(struct Node));
    new -> data = newData;
    /*
    new -> prev = NULL;
    new -> next = *head;
    if ((*head) != NULL) (*head) -> prev = new;
    *head = new;
    /*/
    struct Node *pre = NULL;
    while ((*head) != NULL) {
	    pre = *head;
	    head = &((*head) -> next);
    }
    new -> prev = pre;
    new -> next = NULL;
    *head = new;
    //*/
}

void deleteNode(struct Node** head, int delData) {
    while ((*head) != NULL && (*head) -> data != delData) head = &((*head) -> next);
    if ((*head) -> next != NULL) (*head) -> next -> prev = (*head) -> prev;
    struct Node *del = *head;
    *head = del -> next;
    free(del);
}


void printList(struct Node* head) {
    while (head != NULL) {
        printf("%d <-> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}


void freeList(struct Node* head) {
    while (head != NULL) {
        struct Node* temp = head;
        head = head->next;
        free(temp);
    }
}

int main() {
    struct Node* head = NULL;

    insertNode(&head, 1);
    insertNode(&head, 2);
    insertNode(&head, 3);
    insertNode(&head, 4);
    insertNode(&head, 5);
    insertNode(&head, 6);

    printf("Original Double Linked List: ");
    printList(head);

    deleteNode(&head, 1);

    printf("Double Linked List after deleting node with data 2: ");
    printList(head);

    // Free memory
    freeList(head);

    return 0;
}
