#include<stdio.h>
#include<stdlib.h>
struct LinkedList{
	int data;
	struct LinkedList *next; 
};
void Insert(struct LinkedList** head,int newData){
	struct LinkedList* new = malloc(sizeof(struct LinkedList));
	if (new == NULL) {
		printf("No Memory\n");
		exit(1);
	}
	new -> data = newData;
	new -> next = *head;
	*head = new;
}
void Reverse(struct LinkedList** head)
{
	struct LinkedList* current = *head;
	struct LinkedList* pre = NULL;
	struct LinkedList* next = NULL;
	while (current != NULL) {
		next = current -> next;
		current -> next = pre;
		pre = current;
		current = next;
	}
	*head = pre;
}
void printLinkedList(struct LinkedList* head) {
    while (head != NULL) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}
int main()
{
    struct LinkedList *head = NULL;
	
    Insert(&head,3);
    Insert(&head,7);
    Insert(&head,10); 
    
    
    printf("before:\n");
    printLinkedList(head);
	
    printf("after:\n");
    Reverse(&head);
    printLinkedList(head);
}
