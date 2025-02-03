#include<stdlib.h>
#include<stdio.h>

typedef struct _node {
    int value;
    struct _node *next;
} node;

struct link_list {
    node *first;
    int length;
    void (*append)(struct link_list *, int);
    void (*insert)(struct link_list *, int, int);
    void (*delete)(struct link_list *, int);
    int (*pop)(struct link_list *, int);
    int (*get)(struct link_list *, int);
};

// private
node *jump(struct link_list *list, int index) {
    node *current_node = list -> first;
    for (int i = 1; i <= index; current_node = current_node -> next, i++);
    return current_node;
}

node *add_member(struct link_list *list, int value) {
    node *current_node = (node *)malloc(sizeof(node));
    current_node -> value = value;
    list -> length++;
    return current_node;
}

// method
void append(struct link_list *list, int value) {
    node *pre_node = jump(list, list -> length - 1);
    pre_node -> next = add_member(list, value);
}

void insert(struct link_list *list, int index, int value) {
    node *pre_node = jump(list, index - 1);
    node *next_node = pre_node -> next;
    pre_node -> next = add_member(list, value);
    pre_node -> next -> next = next_node;
}

void delete(struct link_list *list, int index) {
    node *pre_node = jump(list, index - 1);
    pre_node -> next = pre_node -> next -> next;
    list -> length--;
    free(pre_node -> next);
}

int pop(struct link_list *list, int index) {
    node *pre_node = jump(list, index - 1);
    int pop_out = pre_node -> next -> value;
    pre_node -> next = pre_node -> next -> next;
    free(pre_node -> next);
    list -> length--;
    return pop_out;
}

int get(struct link_list *list, int index) {
    return jump(list, index) -> value;
}

// init
struct link_list create_link_list(int value) {
    struct link_list list = {.length = 0};
    list.first = add_member(&list, value);
    list.append = append;
    list.insert = insert;
    list.delete = delete;
    list.pop = pop;
    list.get = get;
    return list;
}

// if insert, delete, pop 到最後一個member  ------- error

int main(void) {
    struct link_list list = create_link_list(-5);
    printf("%d\t", list.length);
    list.append(&list, 20);
    printf("%d\t", list.length);
    list.append(&list, 6);
    printf("%d\t", list.length);
    list.insert(&list, 2, 100);
    printf("%d\t", list.length);
    list.delete(&list, 3);
    printf("%d\t", list.length);
    list.append(&list, 7);
    printf("%d\t", list.length);
    printf("%d  ", list.pop(&list, 3));
    printf("%d\t", list.length);
    list.append(&list, 9);
    printf("%d\n", list.length);

    for (int i = 0; i < list.length; i++) {
        printf("%d\n", get(&list, i));
    }
    return 0;
}
