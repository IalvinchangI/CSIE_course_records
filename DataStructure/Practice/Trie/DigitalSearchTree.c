#include<stdio.h>
#include<stdlib.h>


#define EMPTY (1 << 31)

typedef int* DST;

DST create(int);
void insert(DST, int);
int search(DST, int);
void delete(DST, int);
void print(DST);

int main(void) {
    DST tree = create(3);

    insert(tree, 0);
    insert(tree, 4);
    insert(tree, 5);
    insert(tree, 2);
    insert(tree, 7);
    insert(tree, 3);
    insert(tree, 6);
    insert(tree, 1);
    print(tree);

    delete(tree, 6);
    print(tree);
    
    printf("%d\n", search(tree, 0));
    printf("%d\n", search(tree, 4));
    printf("%d\n", search(tree, 5));
    printf("%d\n", search(tree, 2));
    printf("%d\n", search(tree, 7));
    printf("%d\n", search(tree, 3));
    printf("%d\n", search(tree, 6));
    printf("%d\n", search(tree, 1));

    delete(tree, 4);
    print(tree);
    delete(tree, 2);
    print(tree);
    delete(tree, 7);
    print(tree);
    delete(tree, 0);
    print(tree);

    return 0;
}


DST create(int length) {
    int size = 1 << (length + 1);
    DST out = (DST) malloc(sizeof(int) * size);
    out[0] = length;  // store the length of digital in the index 0

    for (int i = 1; i < size; i++) {
        out[i] = EMPTY;
    }

    return out;
}

void insert(DST tree, int value) {
    int size = 1 << (tree[0] + 1);
    for (int compareBit = 1 << (tree[0] - 1), index = 1; index < size; ) {
        if (tree[index] == value) {
            return;
        }
        if (tree[index] == EMPTY) {
            tree[index] = value;
            return;
        }
        index = (index << 1) + ((compareBit & value) != 0);
        compareBit = compareBit >> 1;
    }
}

int search(DST tree, int value) {
    int size = 1 << (tree[0] + 1);
    for (int compareBit = 1 << (tree[0] - 1), index = 1; index < size; ) {
        if (tree[index] == value) {
            return index;
        }
        if (tree[index] == EMPTY) {
            return 0;
        }
        index = (index << 1) + ((compareBit & value) != 0);
        compareBit = compareBit >> 1;
    }
    return 0;
}

void delete(DST tree, int value) {
    int index = search(tree, value);

    // find leaf
    int size = 1 << (tree[0] + 1);
    int leafIndex = index << 1;
    while (leafIndex < size) {
        if (tree[leafIndex] == EMPTY && tree[leafIndex + 1] == EMPTY) {
            break;
        }
        if (tree[leafIndex + 1] != EMPTY) {
            leafIndex++;
        }
        leafIndex = leafIndex << 1;
    }
    // update delete node
    leafIndex = leafIndex >> 1;
    if (leafIndex == index) {
        tree[index] = EMPTY;
    }
    else {
        tree[index] = tree[leafIndex];
        tree[leafIndex] = EMPTY;
    }
}

void print(DST tree) {
    const int endLevel = tree[0] + 1;
    int index = 1;
    for (int level = 1; level <= endLevel; level++) {
        printf("[ ");
        while (index < (1 << level)) {
            if (tree[index] == EMPTY) {
                printf(" X ");
            }
            else {
                printf("%2d ", tree[index]);
            }
            index++;
        }
        printf(" ]\n");
    }
}
