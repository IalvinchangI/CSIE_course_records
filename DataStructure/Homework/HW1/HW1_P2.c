#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>


// =============================== type =============================== //

#define value_type int

typedef struct __node {
    struct __node *parent;  // parent or next
    value_type value;
    struct __node *children[];  // +2: [left, right]
} _node, *Node;

typedef struct __genericNode {
    struct __genericNode *parent;  // parent or next
    void *value;
    struct __genericNode *children[];  // +2: [left, right]
} _genericNode, *GenericNode;

typedef struct __heap {
    Node root;
    size_t size;
} _heap, *MinHeap;

typedef struct __queue {
    GenericNode head;  // pop
    GenericNode tail;  // push
} _queue, *Queue;


// =============================== prototype of MinHeap =============================== //

MinHeap MinHeap_create(void);
void MinHeap_free(MinHeap);
void MinHeap_insert(MinHeap, value_type);
void MinHeap_delete(MinHeap, value_type);
void MinHeap_print(MinHeap, bool);
static int __MinHeap_get_node_level(int);
static bool __MinHeap_go_right_TF(int, int);


// =============================== prototype of Queue =============================== //

Queue Queue_create(void);
void Queue_free(Queue);
void Queue_push(Queue, void*);
void* Queue_pop(Queue);
bool Queue_isEmpty(Queue);
void Queue_print(Queue);


// =============================== main =============================== //

/**
 * swep two value
 * 
 * @param v1 value1's pointer
 * @param v2 value2's pointer
 */
static void __swep(value_type *v1, value_type *v2) {
    value_type temp = *v2;
    *v2 = *v1;
    *v1 = temp;
}


int main(void) {

    MinHeap minheap = MinHeap_create();

    int value = 0;
    while (1) {
        char input[31] = {0};  // 6(delete/insert) + 1 + 10(int) + 1 + ?(buffer)
        fgets(input, 30, stdin);

        if (input[0] == 0) {
            break;
        }

        if (sscanf(input, "insert %d ", &value) == 1) {
            MinHeap_insert(minheap, value);
        }
        else if (sscanf(input, "delete %d ", &value) == 1) {
            MinHeap_delete(minheap, value);
        }
    }

    MinHeap_print(minheap, false);
    MinHeap_free(minheap);

    return 0;
}



// =============================== define the method of MinHeap =============================== //
/**
 * create a MinHeap
 * 
 * @return MinHeap object
 */
MinHeap MinHeap_create(void){
    MinHeap out = (MinHeap) malloc(sizeof(_heap));
    if (out == NULL) {
        printf(">>> Out of Memory: malloc a new MinHeap <<<");
        exit(EXIT_FAILURE);
    }

    out->root = NULL;
    out->size = 0;

    return out;
}

/**
 * free all the memory in MinHeap
 * 
 * @param minheap MinHeap object
 */
void MinHeap_free(MinHeap minheap) {
    if (minheap->root != NULL) {
        Queue waiting = Queue_create();
        
        Queue_push(waiting, minheap->root);
        while (Queue_isEmpty(waiting) == false) {  // BFS
            // pop
            Node current = (Node) Queue_pop(waiting);

            // push
            for (int i = 0; i < 2; i++) {
                if (current->children[i] != NULL) {
                    Queue_push(waiting, current->children[i]);
                }
            }

            // free
            free(current);
        }
        
        Queue_free(waiting);
    }
    free(minheap);
}

/**
 * insert a value into MinHeap
 * 
 * @param minheap MinHeap object
 * @param value 
 */
void MinHeap_insert(MinHeap minheap, value_type value) {
    Node new = (Node) malloc(sizeof(_node) + sizeof(Node) * 2);  // +2 for children
    if (new == NULL) {
        printf(">>> Out of Memory: malloc a new node of the MinHeap <<<");
        exit(EXIT_FAILURE);
    }

    minheap->size++;  // !!!!!!!!!!!!!! update size !!!!!!!!!!!!!! //
    int newIndex = minheap->size;

    new->value = value;
    new->children[0] = NULL;
    new->children[1] = NULL;

    Node parent = minheap->root;

    // if heap is empty
    if (parent == NULL) {  // insert level 1
        minheap->root = new;
        new->parent = NULL;
        return;
    }

    // else find the parent of end of the heap
    int level = __MinHeap_get_node_level(newIndex);
    int pointer = 1 << (level - 2);
    for (int i = 0; i < (level - 2); i++, pointer >>= 1) {  // level >= 3 will run this loop
        parent = parent->children[__MinHeap_go_right_TF(newIndex, pointer)];
    }

    // insert
    parent->children[__MinHeap_go_right_TF(newIndex, pointer)] = new;
    new->parent = parent;

    // bubble up
    Node current = new;
    while (parent != NULL && parent->value > current->value) {
        __swep(&(parent->value), &(current->value));
        current = parent;
        parent = parent->parent;
    }
}

/**
 * delete a value in MinHeap
 * 
 * @param minheap MinHeap object
 * @param value 
 * @warning If the MinHeap is empty, it will not work correctly. If the value is not in the MinHeap, it will not delete anything.
 */
void MinHeap_delete(MinHeap minheap, value_type value) {
    Node delete = NULL;

    // find
    Queue waiting = Queue_create();
    Queue_push(waiting, minheap->root);
    while (1) {  // BFS
        if (Queue_isEmpty(waiting) == true) {  // <E> value does not exist
            return;
        }

        // pop
        delete = (Node) Queue_pop(waiting);

        if (delete->value == value) {
            break;
        }

        // push
        for (int i = 0; i < 2; i++) {
            if (delete->children[i] != NULL) {
                Queue_push(waiting, delete->children[i]);
            }
        }
    }
    Queue_free(waiting);

    if (minheap->size == 1) {  // MinHeap will be empty
        minheap->root = NULL;

        free(delete);
        minheap->size--;
        return;
    }

    // find end of the heap
    Node end = minheap->root;
    int level = __MinHeap_get_node_level(minheap->size);
    int pointer = 1 << (level - 2);
    for (int i = 0; i < (level - 1); i++, pointer >>= 1) {  // level >= 2 will run this loop
        end = end->children[__MinHeap_go_right_TF(minheap->size, pointer)];
    }

    // delete
    delete->value = end->value;
    end->parent->children[end->parent->children[1] == end] = NULL;

    minheap->size--;
    if (delete == end) {  // delete end
        free(end);
        return;
    }
    free(end);

    // trickle down
    Node current = delete;
    Node child = NULL;
    while (1) {
        if (current->children[0] == NULL) {  // 0 child
            return;
        }
        else if (current->children[1] == NULL) {  // 1 child
            child = current->children[0];
        }
        else {  // 2 children
            child = current->children[current->children[1]->value <= current->children[0]->value];
        }

        if (child->value >= current->value) {
            break;
        }
        // else child < current
        __swep(&(child->value), &(current->value));
        current = child;
    }
}

/**
 * print the MinHeap
 * 
 * @param minheap MinHeap object
 * @param index_TF print index or not
 */
void MinHeap_print(MinHeap minheap, bool index_TF) {
    Queue waiting = Queue_create();
    int index = 0;

    Queue_push(waiting, minheap->root);
    while (Queue_isEmpty(waiting) == false) {  // BFS
        index++;

        // pop
        Node current = (Node) Queue_pop(waiting);
        if (current == NULL) {
            continue;
        }

        // print
        if (index_TF == true) {
            printf("\n[%d]", index);
        }
        printf("%d ", current->value);

        // push
        // in order to calculate index, we will push NULL into Queue
        Queue_push(waiting, current->children[0]);
        Queue_push(waiting, current->children[1]);
    }
    printf("\n");
    
    Queue_free(waiting);
}

/**
 * get the level of the given node
 * 
 * @param index the index of the node
 * @return the level of the node
 */
static int __MinHeap_get_node_level(int index) {
    int test = 1;
    int level = 0;
    while (test <= index) {
        // test whether the index is less then the index of the leftist node in certain level
        test <<= 1;
        level++;
    }
    return level;
}

/**
 * Decide to go right or left
 * 
 * @param index 
 * @param pointer 
 * @return true if go right else false
 */
static bool __MinHeap_go_right_TF(int index, int pointer) {
    bool out = index & pointer;
    return out;
}


// =============================== define the method of Queue =============================== //
/**
 * create a Queue
 * 
 * @return Queue object
 */
Queue Queue_create(void) {
    Queue out = (Queue) malloc(sizeof(_queue));
    if (out == NULL) {
        printf(">>> Out of Memory: malloc a new Queue <<<");
        exit(EXIT_FAILURE);
    }

    out->head = NULL;
    out->tail = NULL;

    return out;
}

/**
 * free all the memory in Queue
 * 
 * @param object Queue object
 */
void Queue_free(Queue queue) {
    GenericNode curtent = queue->head;
    GenericNode temp = NULL;
    while (curtent != NULL) {
        temp = curtent;
        curtent = temp->parent;
        free(temp);
    }
    free(queue);
}

/**
 * push the value into the Queue
 * 
 * @param queue Queue object
 * @param value 
 */
void Queue_push(Queue queue, void* value) {
    GenericNode new = (GenericNode) malloc(sizeof(_genericNode));
    if (new == NULL) {
        printf(">>> Out of Memory: malloc a new node of the Queue <<<");
        exit(EXIT_FAILURE);
    }

    new->value = value;
    new->parent = NULL;  // push to the end of the queue

    if (queue->head == NULL) {  // Queue is empty
        queue->head = new;
    }
    else {
        queue->tail->parent = new;  // tail links to new
    }
    queue->tail = new;
}

/**
 * pop out the top element in the Queue
 * 
 * @param queue Queue object
 * @return the value of the top element
 * @warning if the Queue is empty, it will not work correctly
 */
void* Queue_pop(Queue queue) {
    GenericNode out = queue->head;
    void *value = out->value;

    queue->head = out->parent;
    if (queue->head == NULL) {  // Queue will be empty
        queue->tail = NULL;
    }

    free(out);
    return value;
}

/**
 * check whether the Queue is empty
 * 
 * @param queue Queue object
 * @return true if the Queue is empty else false
 */
bool Queue_isEmpty(Queue queue) {
    return queue->head == NULL;  // queue->tail == NULL
}

/**
 * print the Queue
 * 
 * @param queue Queue object
 */
void Queue_print(Queue queue) {
    printf("<%p>: [pop] <- ", queue);

    GenericNode curtent = queue->head;
    while (curtent != NULL) {
        printf("%p <- ", curtent->value);
        curtent = curtent->parent;
    }
    
    printf("[push]\n");
}
