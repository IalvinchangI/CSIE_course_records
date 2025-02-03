#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>


// =============================== type =============================== //

typedef struct __graph {
    size_t size;
    char** matrix;
} _graph, *Graph;

typedef void* GenericType;

typedef struct __queue {
    size_t size;
    int head;  // pop
    int rear;  // push
    GenericType *container;
} _queue, *Queue;


// =============================== prototype of Graph =============================== //

Graph Graph_create(size_t);
void Graph_free(Graph);
void Graph_insertEdge(Graph, unsigned int, unsigned int);
void Graph_deleteEdge(Graph, unsigned int, unsigned int);
void Graph_print(Graph);

void Graph_getNeighbors(Graph, unsigned int, Queue);


// =============================== prototype of Queue =============================== //

Queue Queue_create(size_t);
void Queue_free(Queue);
void Queue_push(Queue, GenericType);
GenericType Queue_pop(Queue);
bool Queue_isEmpty(Queue);
bool Queue_isFull(Queue);
static void Queue_new_space(Queue);

void Queue_print_uint(Queue, bool);
void Queue_push_uint(Queue, unsigned int);
unsigned int Queue_pop_uint(Queue);


// =============================== main =============================== //

int main(void) {
    // read input
    int size = 0;
    if (scanf("%d", &size) != 1) {
        exit(EXIT_FAILURE);
    }
    Graph graph = Graph_create(size);
    
    for (int i = 1; i <= size; i++) {
        for (int j = 1; j <= size; j++) {
            int connect_TF = 0;
            if (scanf("%d", &connect_TF) != 1) {
                exit(EXIT_FAILURE);
            }
            if (connect_TF == 1) {
                Graph_insertEdge(graph, i, j);
            }
        }
    }

    // BFS
    Queue waiting = Queue_create(12);
    Queue result = Queue_create(12);
    char* visited = (char*) calloc(size, sizeof(char));
    if (visited == NULL) {
        printf(">>> Out of Memory: malloc visited <<<");
        exit(EXIT_FAILURE);
    }
    visited--;

    Queue_push_uint(waiting, 1);
    while (Queue_isEmpty(waiting) == false) {
        // pop
        unsigned int vertex = Queue_pop_uint(waiting);

        if (visited[vertex] == 0) {
            // record
            Queue_push_uint(result, vertex);
            visited[vertex] = 1;

            // push
            Graph_getNeighbors(graph, vertex, waiting);
        }
    }

    // result
    Queue_print_uint(result, false);

    free(visited + 1);
    Queue_free(waiting);
    Queue_free(result);
    Graph_free(graph);

    return 0;
}




// =============================== define the method of Graph =============================== //
/**
 * create a Graph
 * 
 * @return Graph object
 */
Graph Graph_create(size_t vertexCount) {
    Graph out = (Graph) malloc(sizeof(_graph));
    if (out == NULL) {
        printf(">>> Out of Memory: malloc a new Graph <<<");
        exit(EXIT_FAILURE);
    }

    out->matrix = (char**) malloc(sizeof(char*) * vertexCount);
    if (out->matrix == NULL) {
        printf(">>> Out of Memory: malloc a new Graph <<<");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < vertexCount; i++) {
        out->matrix[i] = (char*) calloc(vertexCount, sizeof(char));
        if (out->matrix[i] == NULL) {
            printf(">>> Out of Memory: malloc a new Graph <<<");
            exit(EXIT_FAILURE);
        }
        out->matrix[i]--;
    }
    out->matrix--;

    out->size = vertexCount;

    return out;
}

/**
 * free all the memory in Graph
 * 
 * @param graph Graph object
 */
void Graph_free(Graph graph) {
    for (int i = 1; i <= graph->size; i++) {
        free(graph->matrix[i] + 1);
    }
    free(graph->matrix + 1);

    free(graph);
}

/**
 * connect two vertices by edge
 * 
 * @param graph Graph object
 * @param vertex1 the head of edge
 * @param vertex2 the tail of edge
 */
void Graph_insertEdge(Graph graph, unsigned int vertex1, unsigned int vertex2) {
    graph->matrix[vertex1][vertex2] = 1;
}

/**
 * delete the edge between two vertices
 * 
 * @param graph Graph object
 * @param vertex1 the head of edge
 * @param vertex2 the tail of edge
 */
void Graph_deleteEdge(Graph graph, unsigned int vertex1, unsigned int vertex2) {
    graph->matrix[vertex1][vertex2] = 0;
}

/**
 * print the Graph
 * 
 * @param graph Graph object
 */
void Graph_print(Graph graph) {
    for (int i = 1; i <= graph->size; i++) {
        for (int j = 1; j <= graph->size; j++) {
            printf("%d ", graph->matrix[i][j]);
        }
        printf("\n");
    }
}


/**
 * get the neighbors of vertex1
 * 
 * @param graph Graph object
 * @param vertex1 
 * @param out where to push the result
 */
void Graph_getNeighbors(Graph graph, unsigned int vertex1, Queue out) {
    for (int i = 1; i <= graph->size; i++) {
        if (graph->matrix[vertex1][i] == 1) {
            Queue_push_uint(out, i);
        }
    }
}


// =============================== define the method of Queue =============================== //
/**
 * create a Queue
 * 
 * @return Queue object
 */
Queue Queue_create(size_t size) {
    Queue out = (Queue) malloc(sizeof(_queue));
    if (out == NULL) {
        printf(">>> Out of Memory: malloc a new Queue <<<");
        exit(EXIT_FAILURE);
    }
    out->size = size + 1;

    out->container = (GenericType*) malloc(sizeof(GenericType) * out->size);
    if (out == NULL) {
        printf(">>> Out of Memory: malloc a new Queue <<<");
        exit(EXIT_FAILURE);
    }

    out->head = 0;
    out->rear = 0;

    return out;
}

/**
 * free all the memory in Queue
 * 
 * @param object Queue object
 * @warning This will delete all the data in the Queue
 */
void Queue_free(Queue queue) {
    for (int i = queue->head; i != queue->rear; i = (i + 1) % queue->size) {
        free(queue->container[i]);
    }
    free(queue);
}

/**
 * push the value into the Queue
 * 
 * @param queue Queue object
 * @param value 
 * @warning If queue is full, it will ralloc.
 */
void Queue_push(Queue queue, GenericType value) {
    if (Queue_isFull(queue) == true) {
        Queue_new_space(queue);
    }

    queue->container[queue->rear] = value;
    queue->rear = (queue->rear + 1) % queue->size;
}

/**
 * pop out the top element in the Queue
 * 
 * @param queue Queue object
 * @return the value of the top element; If queue is empty, return NULL.
 */
GenericType Queue_pop(Queue queue) {
    if (Queue_isEmpty(queue) == true) {
        return NULL;
    }

    GenericType out = queue->container[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    return out;
}

/**
 * check whether the Queue is empty
 * 
 * @param queue Queue object
 * @return true if the Queue is empty else false
 */
bool Queue_isEmpty(Queue queue) {
    return queue->head == queue->rear;
}

/**
 * check whether the Queue is full
 * 
 * @param queue Queue object
 * @return true if the Queue is full else false
 */
bool Queue_isFull(Queue queue) {
    return (queue->rear + 1) % queue->size == queue->head;
}

/**
 * require new memory for Queue
 * 
 * @param queue Queue object
 */
static void Queue_new_space(Queue queue) {
    GenericType *newContainer = (GenericType*) malloc(sizeof(GenericType) * (queue->size << 1));
    if (newContainer == NULL) {
        printf(">>> Out of Memory: malloc new space for Queue <<<");
        exit(EXIT_FAILURE);
    }

    // copy
    int newRear = 0;
    for (int i = queue->head; i != queue->rear; i = (i + 1) % queue->size, newRear++) {
        newContainer[newRear] = queue->container[i];
    }

    // setting
    queue->head = 0;
    queue->rear = newRear;
    queue->size <<= 1;
    free(queue->container);
    queue->container = newContainer;
}

/**
 * print the Queue
 * 
 * @param queue Queue object
 * @param showArrow_TF show arrow or not
 */
void Queue_print_uint(Queue queue, bool showArrow_TF) {
    if (showArrow_TF == true) {
        printf("<%p>: [pop] <- ", queue);
    }

    for (int i = queue->head; i != queue->rear; i = (i + 1) % queue->size) {
        printf("%u ", *((unsigned int*) (queue->container[i])));
        if (showArrow_TF == true) {
            printf("<- ");
        }
    }
    
    if (showArrow_TF == true) {
        printf("[push]\n");
    }
}

/**
 * push the value into the Queue
 * 
 * @param queue Queue object
 * @param value 
 * @warning If queue is full, it will ralloc.
 */
void Queue_push_uint(Queue queue, unsigned int value) {
    unsigned int *valueP = (unsigned int*) malloc(sizeof(unsigned int));
    *valueP = value;
    Queue_push(queue, valueP);
}

/**
 * pop out the top element in the Queue
 * 
 * @param queue Queue object
 * @return the value of the top element; If queue is empty, return NULL.
 */
unsigned int Queue_pop_uint(Queue queue) {
    unsigned int* valueP = Queue_pop(queue);
    unsigned int value = *valueP;

    free(valueP);
    return value;
}
