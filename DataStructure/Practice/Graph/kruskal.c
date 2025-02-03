#include<stdlib.h>
#include<stdio.h>
#include<string.h>


#define VERTEX_COUNT 5

typedef struct __line {
    int weight;
    int vertices[2];
} _Line, *Line;

int compare(const void **a, const void **b) {
    return ((Line)*a)->weight - ((Line)*b)->weight;
}

int find(int sets[], int set) {
    int current = set;
    while (sets[current] >= 0) {
        current = sets[current];
    }
    return current;
}


int main(void) {
    int graph[VERTEX_COUNT][VERTEX_COUNT] = {
        {0, 0, 5, 0, 9},  // 0
        {0, 0, 0, 2, 3},  // 1
        {5, 0, 0, 7, 0},  // 2
        {0, 2, 7, 0, 1},  // 3
        {9, 3, 0, 1, 0},  // 4
    };
    Line edges[VERTEX_COUNT * VERTEX_COUNT] = {0};
    int index = 0;
    for (int i = 0; i < VERTEX_COUNT; i++) {
        for (int j = i + 1; j < VERTEX_COUNT; j++) {
            if (graph[i][j] != 0) {
                edges[index] = (Line) malloc(sizeof(_Line));
                edges[index]->vertices[0] = i;
                edges[index]->vertices[1] = j;
                edges[index]->weight = graph[i][j];
                index++;
            }
        }
    }
    int edgeSize = index;
    qsort(edges, edgeSize, sizeof(Line), compare);


    int disjointSet[VERTEX_COUNT];
    memset(disjointSet, -1, VERTEX_COUNT * sizeof(int));
    for (int i = 0; i < edgeSize; i++) {
        if (find(disjointSet, edges[i]->vertices[0]) == find(disjointSet, edges[i]->vertices[1])) {
            continue;
        }
        disjointSet[find(disjointSet, edges[i]->vertices[1])] = edges[i]->vertices[0];
        printf("%d <-> %d : %d\n", edges[i]->vertices[0], edges[i]->vertices[1], edges[i]->weight);
    }

    return 0;
}

