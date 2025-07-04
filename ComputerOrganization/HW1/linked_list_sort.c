#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} Node;

// Split the linked list into two parts
void splitList(Node *head, Node **firstHalf, Node **secondHalf)
{
    __asm__ volatile(
        // Block A (splitList), which splits the linked list into two halves
        // t0 = cur
        // t1 = firstTail
        // t2 = firstTail->next
        "ld t0, 8(%[head])\n"  // cur = head->next = head + 8bytes
        "add t1, %[head], x0\n"  // firstTail = head
        "SPLIT_WHILE_LOOP:\n"
            "ld t0, 8(t0)\n"  // cur = cur->next = cur + 8bytes
            "beq t0, x0, END_SPLIT_WHILE_LOOP\n"
            "ld t1, 8(t1)\n"  // firstTail = firstTail->next = firstTail + 8bytes

            "ld t0, 8(t0)\n"  // cur = cur->next = cur + 8bytes
            "beq t0, x0, END_SPLIT_WHILE_LOOP\n"
            "jal x0, SPLIT_WHILE_LOOP\n"
        "END_SPLIT_WHILE_LOOP:\n"
        "sd %[head], 0(%[firstHalf])\n"  // *firstHalf = head
        "ld t2, 8(t1)\n"  // t2 = t1->next = t1 + 8bytes = firstTail + 8bytes
        "sd t2, 0(%[secondHalf])\n"  // *secondHalf = firstTail->next
        "sd x0, 8(t1)\n"  // firstTail->next = firstTail + 8bytes = NULL
        :
        [firstHalf] "+r"(firstHalf), [secondHalf] "+r"(secondHalf) :
        [head] "r"(head) :
        "t0", "t1", "t2", "memory"
    );
}

// Merge two sorted linked lists
Node *mergeSortedLists(Node *a, Node *b)
{
    Node *result = NULL;
    Node *tail = NULL;
    
    __asm__ volatile(
        // Block B (mergeSortedList), which merges two sorted lists into one
        // t0 = a->data
        // t1 = b->data
        "lw t0, 0(%[a])\n"
        "lw t1, 0(%[b])\n"
        "blt t0, t1, IF1\n"
        // else
            "add %[result], %[b], x0\n"  // result = b
            "add %[tail], %[b], x0\n"  // tail = b
            "ld %[b], 8(%[b])\n"  // b = b->next = b + 8bytes
            "beq %[b], x0, ELSE3\n"
            "lw t1, 0(%[b])\n"  // t1 = b->data
            "jal x0, END_IF1\n"
        "IF1:\n"  // if
            "add %[result], %[a], x0\n"  // result = a
            "add %[tail], %[a], x0\n"  // tail = a
            "ld %[a], 8(%[a])\n"  // a = a->next = a + 8bytes
            "beq %[a], x0, IF3\n"
            "lw t0, 0(%[a])\n"  // t0 = a->data
        "END_IF1:\n"
        "WHILE_LOOP:\n"
            "blt t0, t1, IF2\n"
            // else
                "sd %[b], 8(%[tail])\n"  // tail->next = tail + 8bytes = b
                "add %[tail], %[b], x0\n"  // tail = b
                "ld %[b], 8(%[b])\n"  // b = b->next = b + 8bytes
                "beq %[b], x0, ELSE3\n"
                "lw t1, 0(%[b])\n"  // t1 = b->data
                "jal x0, END_IF2\n"
            "IF2:\n"  // if
                "sd %[a], 8(%[tail])\n"  // tail->next = tail + 8bytes = a
                "add %[tail], %[a], x0\n"  // tail = a
                "ld %[a], 8(%[a])\n"  // a = a->next = a + 8bytes
                "beq %[a], x0, IF3\n"
                "lw t0, 0(%[a])\n"  // t0 = a->data
            "END_IF2:\n"
            "jal x0, WHILE_LOOP\n"
        "END_WHILE_LOOP:\n"
        "ELSE3:\n"  // else
            "sd %[a], 8(%[tail])\n"  // tail->next = tail + 8bytes = a
            "jal x0, END_IF3\n"
        "IF3:\n"  // if
            "sd %[b], 8(%[tail])\n"  // tail->next = tail + 8bytes = b
        "END_IF3:\n"
        :
        [result] "+r"(result), [tail] "+r"(tail), [a] "+r"(a), [b] "+r"(b) :
        :
        "t0", "t1", "memory"
    );

    return result;
}

// Merge Sort function for linked list
Node *mergeSort(Node *head)
{
    if (!head || !head->next)
        return head; // Return directly if there is only one node

    Node *firstHalf, *secondHalf;
    splitList(head, &firstHalf,
              &secondHalf); // Split the list into two sublists

    firstHalf = mergeSort(firstHalf);   // Recursively sort the left half
    secondHalf = mergeSort(secondHalf); // Recursively sort the right half

    return mergeSortedLists(firstHalf, secondHalf); // Merge the sorted sublists
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        return 1;
    }
    int list_size;
    fscanf(input, "%d", &list_size);
    Node *head = (list_size > 0) ? (Node *)malloc(sizeof(Node)) : NULL;
    Node *cur = head;
    for (int i = 0; i < list_size; i++) {
        fscanf(input, "%d", &(cur->data));
        if (i + 1 < list_size)
            cur->next = (Node *)malloc(sizeof(Node));
        cur = cur->next;
    }
    fclose(input);

    // Linked list sort
    head = mergeSort(head);
    
    cur = head;
    while (cur) {
        printf("%d ", cur->data);
        __asm__ volatile(
            // Block C (Move to the next node), which updates the pointer to
            // traverse the linked list
            "ld %[cur], 8(%[cur])\n"  // cur = cur->next = cur + 8bytes
            :
            [cur] "+r"(cur)
        );
    }
    printf("\n");
    return 0;
}
