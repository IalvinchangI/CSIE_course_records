#include<stdlib.h>
#include<stdio.h>


typedef struct __heap {
    int size;
    int capacity;
    int* container;
} _Heap, *Heap;


Heap create(int size);
void insert(Heap heap, int value);
void delete(Heap heap, int value);

void heapifyUp(Heap heap, int index);
void heapifyDown(Heap heap, int index);

void print(Heap heap);


int main(void) {
    Heap heap = create(10);

    insert(heap, 3);
    print(heap);
    insert(heap, 30);
    print(heap);
    insert(heap, 15);
    print(heap);
    insert(heap, 4);
    print(heap);
    insert(heap, 7);
    print(heap);
    insert(heap, 18);
    print(heap);
    delete(heap, 3);
    print(heap);
    delete(heap, 7);
    print(heap);

    return 0;
}


Heap create(int capacity) {
    Heap out = malloc(sizeof(_Heap));
    out->container = malloc(sizeof(int) * (capacity + 1));
    out->capacity = capacity;
    out->size = 0;
    return out;
}

void insert(Heap heap, int value) {
    // insert
    heap->size++;
    heap->container[heap->size] = value;

    // heapify up
    heapifyUp(heap, heap->size);
}

void delete(Heap heap, int value) {
    // find
    int index = 0;
    for (int i = 1; i <= heap->size; i++) {
        if (heap->container[i] == value) {
            index = i;
            break;
        }
    }
    if (index == 0) {
        printf("Not Found\n");
        return;
    }

    // delete
    heap->container[index] = heap->container[heap->size];
    heap->size--;

    // heapify
    // if (index > heap->size) {
    //     return;
    // }
    int current_value = heap->container[index];
    if (index > 1 && current_value < heap->container[index >> 1]) {  // define min heap
        heapifyUp(heap, index);
    }
    else {
        heapifyDown(heap, index);
    }
}


void heapifyUp(Heap heap, int index) {
    int current = index;
    int parent = index >> 1;
    while (parent >= 1 && heap->container[current] < heap->container[parent]) {  // define min heap
        // exchange
        int temp = heap->container[current];
        heap->container[current] = heap->container[parent];
        heap->container[parent] = temp;

        // next
        current = parent;
        parent = current >> 1;
    }
}

void heapifyDown(Heap heap, int index) {
    int current = index;
    int minChild = index << 1;  // assume min child is left child
    while (minChild <= heap->size) {
        // find min child
        if ((minChild + 1 <= heap->size) && (heap->container[minChild] > heap->container[minChild + 1])) {  // define min heap
            minChild++;  // right child is min child
        }

        // compare
        if (heap->container[current] <= heap->container[minChild]) {  // define min heap
            break;  // current <= minChild -> stop
        }
        
        // exchange
        int temp = heap->container[current];
        heap->container[current] = heap->container[minChild];
        heap->container[minChild] = temp;

        // next
        current = minChild;
        minChild = current << 1;
    }
}

void print(Heap heap) {
    for (int i = 1; i <= heap->size; i++) {
        printf("\n[%d] %d\n", i, heap->container[i]);
    }
    printf("\n");
}
