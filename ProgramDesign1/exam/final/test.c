// ConsoleApplication1.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//int f(int a, float b) {
//    return (int)b + a;
//}

//typedef int (*G)(int, float);

struct Node {
    int data;
    struct Node* next;
    struct Node* prev;
};

typedef struct Node node;

void insert(struct Node** head, struct Node* node) {
    // put node into list, order them
    struct Node* pre = NULL;
    while (*head != NULL && ((*head)->data) < (node->data)) {  // find
        pre = *head;
        head = &((*head)->next);
    }
    // insert
    node->prev = pre;
    node->next = *head;
    if (*head != NULL) (*head)->prev = node;
    *head = node;
}

/*
void splitLinkedList(struct Node** head, struct Node** out_odd, struct Node** out_even) {
    struct Node* current = *head;
    while (current != NULL) {
        struct Node* next = current->next;  // temp

        if ((current->data) & 1) {  // odd
            insert(out_odd, current);
        }
        else {  // even
            insert(out_even, current);
        }
        current = next;
    }
}
*/

/*
void reverse(node** h) {
    node* pre = NULL, * next = NULL;

    while (*h != NULL) {
        next = (*h)->next;
        (*h)->next = pre;
        pre = *h;
        *h = next;
    }
    *h = pre;
}
*/
/*
void reverse(node** h) {
    static node* pre = NULL;
    if (*h != NULL) {
        node* next = (*h)->next;
        (*h)->next = pre;
        pre = *h;
        *h = next;
        reverse(h);
    }
    else {
        *h = pre;
        pre = NULL;
    }
}
*/

void print_list(const struct Node* head) {
    while (head != NULL) {
        printf("%d -> ", head->data);
        head = head->next;
    }
    printf("NULL\n");
}

int cmpstr(void* a, void* b) {
    return strcmp(*(char**)a, *(char**)b);
}

int cmpnum(void* a, void* b) {
    return *(int*)a - * (int*)b;
}

void copy(void* a, void* b, int size) {
    for (int i = 0; i < size; i++) {
        *((char*)a + i) = *((char*)b + i);
    }
}

int splite(char* a, int size, int low, int high, int (*cmp)(void*, void*)) {
    char* part = a + size * low;
    while (1) {
        while (low < high && (cmp(part, a + size * high) <= 0)) high--;
        if (low >= high) break;
        while (low < high && (cmp(a + size * low, part) <= 0)) low++;
        if (low >= high) break;
        copy(a + size * high, a + size * low, size);
        high--;
    }
    copy(a + size * high, part, size);
    return high;
}

void _qsort(void* a, int size, int low, int high, int (*cmp)(void*, void*)) {
    int middle;

    if (low >= high) return;
    char* ca = (char*)a;
    middle = splite(ca, size, low, high, cmp);
    for (int i = 0; i < 7; i++) {
        printf("%d, ", ((int*)a)[i]);
    }
    printf("\n");
    _qsort(a, size, low, middle - 1, cmp);
    _qsort(a, size, middle, high, cmp);
}

void convert_prefix(char* ipv6, int len, unsigned* prefix, unsigned char* prefixlen) {
    unsigned a[8] = { 0 };
    sscanf(ipv6, "%x:%x:%x:%x:%x:%x:%x:%x/%u", a, a + 1, a + 2, a + 3, a + 4, a + 5, a + 6, a + 7, prefixlen);
    for (int i = 0; i < 4; i++) {
        prefix[i] = (a[i * 2] << 16) | a[i * 2 + 1];
    }
}

a = (~0) << (128 - len);


int main(void) {
    /*
    G* h_2[3] = {0};
    (h_2[1]) = &f;
    printf("%d\n", (*h_2[1])(1, 2.1f));
    */
    /*
    struct Node* head = NULL;
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = 1;
    insert(&head, new_node);

    new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = 5;
    insert(&head, new_node);

    new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = 2;
    insert(&head, new_node);

    new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = 4;
    insert(&head, new_node);

    new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->data = 7;
    insert(&head, new_node);

    print_list(head);
    struct Node* even = NULL;
    struct Node* odd = NULL;
    splitLinkedList(&head, &odd, &even);
    print_list(odd);
    print_list(even);

    reverse(&head);
    print_list(head);

    reverse(&head);
    print_list(head);
    */

    /*
    const char* a[] = { "bbc", "xcd", "ede", "def", "afg", "hello", "hmmm", "okay", "how" };

    int b[] = { 45, 78, 89, 65, 70, 23, 44 };
    int* p = b;
    //_qsort(a, sizeof(char*), 0, 8, (int (*)(void*, void*))(cmpstr));
    _qsort(p, sizeof(int), 0, 6, (int (*)(void*, void*))(cmpnum));

    for (int i = 0; i < 7; i++) {
        printf("%d, ", b[i]);
    }
    printf("\n");

    for (int i = 0; i < 9; i++) {
        printf("%s, ", a[i]);
    }
    printf("\n");
    */

    convert_prefix()

    return 0;
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
