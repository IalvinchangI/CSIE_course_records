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

void Delete(struct LinkedList** head,int delData)
{
	struct LinkedList* current = *head;
	struct LinkedList* pre = NULL;
	while (1) {
		if (current == NULL) {
			printf("Does not exist\n");
			return;
		}
		if (current -> data == delData) break;
		pre = current;
		current = current -> next;
	}
	if (pre == NULL) {
		*head = current -> next;
	} else {
		pre -> next = current -> next;
	}
	free(current);
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
    struct LinkedList *head=NULL;
    int i,del_num;
    
    for(i=0;i<10;i++)
    {
        Insert(&head,i);
    }
    printLinkedList(head);
	
    printf("Enter del_num:");
    scanf("%d",&del_num);	
    Delete(&head,del_num);
    printLinkedList(head);
	
}

