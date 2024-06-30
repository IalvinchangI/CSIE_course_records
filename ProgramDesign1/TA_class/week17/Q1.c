#include<stdio.h>
#include<stdlib.h>

struct LinkedList{
    int data;
    struct LinkedList *next; 
};

struct LinkedList* Insert(struct LinkedList* head,int newData){ 
    struct LinkedList *newNode = malloc(sizeof(struct LinkedList));
    newNode -> data = newData;
    /*
    newNode -> next = head;
    return newNode;
    /*/
    struct LinkedList **current = &head;
    // while (*current != NULL) current = &((*current) -> next);
    while (*current != NULL && *(current = &((*current) -> next)));
    *current = newNode;
    return head;
    //*/
}

void printLinkedList(struct LinkedList* head) {
    while (head != NULL) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}

void deleteMiddle(struct LinkedList* head){
    int count = 0;
    struct LinkedList *current = head;
    while (current != NULL) {
	   current = current -> next;
	   count++;
    }
    current = head;
    for (int i = 0; i < (count / 2 - 1); i++) current = current -> next;
    struct LinkedList *del = current -> next;
    current -> next = current -> next -> next;
    free(del);
}

int main(){
    struct LinkedList *head=NULL;
    int i, total, num;
    scanf("%d", &total);
    for(i = 0; i < total; i++){
        scanf("%d", &num);
        head = Insert(head,num);
    }
    printLinkedList(head);
    deleteMiddle(head);
    printLinkedList(head);
}
