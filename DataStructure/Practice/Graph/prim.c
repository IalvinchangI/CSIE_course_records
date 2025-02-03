#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<limits.h>


#define VERTEX_COUNT 5

typedef struct __line {
    int weight;
    int vertices[2];
} _Line, *Line;

int compare(const void **a, const void **b) {
    return ((Line)*a)->weight - ((Line)*b)->weight;
}



int main(void) {
    int graph[VERTEX_COUNT][VERTEX_COUNT] = {
        {0, 0, 5, 0, 9},  // 0
        {0, 0, 0, 2, 3},  // 1
        {5, 0, 0, 7, 0},  // 2
        {0, 2, 7, 0, 1},  // 3
        {9, 3, 0, 1, 0},  // 4
    };

    Line queue[VERTEX_COUNT * VERTEX_COUNT] = {0};
    int size = 0;
    
    int startVertex = 0;

    int visited[VERTEX_COUNT] = {0};
    visited[startVertex] = 1;
    while (1) {
        for (int i = 0; i < VERTEX_COUNT; i++) {
            if (graph[startVertex][i] > 0 && visited[i] == 0) {
                queue[size] = (Line) malloc(sizeof(_Line));
                queue[size]->vertices[0] = startVertex;
                queue[size]->vertices[1] = i;
                queue[size]->weight = graph[startVertex][i];
                size++;
            }
        }
        qsort(queue, size, sizeof(Line), compare);
        if (queue[0]->weight == INT_MAX) {
            break;
        }

        Line small = NULL;
        for (int i = 0; i < size; i++) {
            small = queue[i];
            if (visited[small->vertices[1]] == 0) {
                printf("%d <-> %d : %d\n", small->vertices[0], small->vertices[1], small->weight);
                visited[small->vertices[1]] = 1;
                startVertex = small->vertices[1];
                small->weight = INT_MAX;
                break;
            }
            small->weight = INT_MAX;
        }
    }

    return 0;
}
