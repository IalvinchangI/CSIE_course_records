#include<stdlib.h>
#include<stdio.h>
#include<string.h>


typedef struct __disjointSet {
    int size;
    int* container;
} _DisjointSet, *DisjointSet;

DisjointSet create_set(int size);
void union_set(DisjointSet sets, int set1, int set2);
int find_set(DisjointSet sets, int set);
void print_set(DisjointSet sets);



int main(void) {
    DisjointSet sets = create_set(5);

    union_set(sets, 1, 4);
    print_set(sets);
    union_set(sets, 3, 2);
    print_set(sets);
    union_set(sets, 4, 1);
    print_set(sets);
    union_set(sets, 2, 1);
    print_set(sets);
    union_set(sets, 0, 4);
    print_set(sets);
    find_set(sets, 3);
    print_set(sets);


    return 0;
}



DisjointSet create_set(int size) {
    DisjointSet out = malloc(sizeof(_DisjointSet));
    out->container = malloc(sizeof(int) * size);
    memset(out->container, -1, sizeof(int) * size);
    out->size = size;
    return out;
}

void union_set(DisjointSet sets, int set1, int set2) {
    // find root
    set1 = (set1 >= 0) ? find_set(sets, set1) : set1;
    set2 = (set2 >= 0) ? find_set(sets, set2) : set2;
    if (set1 == set2) {
        return;
    }

    // union by height rule
    if (sets->container[set1] < sets->container[set2]) {  // height: set1 > set2
        // we won't update the height of set1
        sets->container[set2] = set1;  // union 2 -> 1
    }
    else if (sets->container[set1] > sets->container[set2]) {  // height: set1 < set2
        // we won't update the height of set2
        sets->container[set1] = set2;  // union 1 -> 2
    }
    else {  // height: set1 == set2
        sets->container[set1]--;  // update height
        sets->container[set2] = set1;  // union 2 -> 1
    }
}

int find_set(DisjointSet sets, int set) {
    int current = set;
    while (sets->container[current] >= 0) {
        current = sets->container[current];
    }

    int root = current;
    current = set;
    while (current != root && sets->container[current] != root) {
        int subRoot = sets->container[current];
        sets->container[current] = root;
        current = subRoot;
        // ignore the height change
    }
    return root;
}

void print_set(DisjointSet sets) {
    for (int i = 0; i < sets->size; i++) {
        printf("%d\t%d\n", i, sets->container[i]);
    }
    printf("\n");
}
