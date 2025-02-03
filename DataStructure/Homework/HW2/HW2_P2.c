#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define private static
#define public


// =============================== type =============================== //

// key type
#define ktype int


// FHeap node
typedef struct __fheapNode {
    ktype key;
    struct __fheapNode* parent;       // parent node
    struct __fheapNode* siblings[2];  // 0: left sibling; 1: right sibling
    struct __fheapNode* child;        // child node
    int degree;
    bool childCutFlag;
} _FHeapNode, *FHeapNode;

typedef FHeapNode *FHeap;


// queue node
typedef struct __node {
    void* value;
    struct __node* next;
} _Node, *Node;

typedef struct __queue {
    Node head;  // pop
    Node tail;  // push
} _queue, *Queue;


// =============================== prototype of MinHeap =============================== //

public FHeap FHeap_create();
public void FHeap_free(FHeap);
public void FHeap_insert(FHeap, ktype);
public void FHeap_delete(FHeap, ktype);
public void FHeap_deleteMin(FHeap);
public void FHeap_decreaseKey(FHeap, ktype, ktype);
public void FHeap_print(FHeap);

private FHeapNode __FHeap_sortLevel(FHeapNode, int (*key)(FHeapNode, FHeapNode));
private int __FHeap_sortLevelKey_key(FHeapNode, FHeapNode);
private int __FHeap_sortLevelKey_degree(FHeapNode, FHeapNode);
private void __FHeap_linkNode(FHeapNode, FHeapNode);

private FHeapNode __FHeap_sortRootLevel(FHeap);
private FHeapNode __FHeap_findMinNode(FHeapNode, FHeapNode);
private FHeapNode __FHeap_findNode(FHeap, ktype);
private void __FHeap_minTreeJoining(FHeap);
private void __FHeap_cascadingCut(FHeap, FHeapNode);


// =============================== prototype of Queue =============================== //

public Queue Queue_create(void);
public void Queue_free(Queue);
public void Queue_push(Queue, void*);
public void* Queue_pop(Queue);
public bool Queue_isEmpty(Queue);
public void Queue_print(Queue);


int main(void) {
    FHeap heap = FHeap_create();

    int key = 0;
    int value = 0;
    while (1) {
        char input[26] = {0};  // 8(delete/insert/decrease) + 1 + 5(int) + 1 + 5(int) + 1 + ?(buffer) || 11(extract-min/exit) + 1
        scanf("%s", input);

        if (strcmp(input, "exit") == 0) {
            break;
        }
        else if (strcmp(input, "extract-min") == 0) {
            FHeap_deleteMin(heap);
            FHeap_print(heap);
        }
        else if (strcmp(input, "insert") == 0) {
            scanf("%d", &key);
            FHeap_insert(heap, key);
            FHeap_print(heap);
        }
        else if (strcmp(input, "delete") == 0) {
            scanf("%d", &key);
            FHeap_delete(heap, key);
            FHeap_print(heap);
        }
        else if (strcmp(input, "decrease") == 0) {
            scanf("%d %d", &key, &value);
            FHeap_decreaseKey(heap, key, value);
            // FHeap_print(heap);
        }
        else {  // not support
            break;
        }
    }

    FHeap_print(heap);
    FHeap_free(heap);
    
    return 0;
}




// =============================== define the method of FHeap =============================== //

/**
 * @brief create a FHeap
 * 
 * @return FHeap object
 */
public FHeap FHeap_create() {
    FHeap root = (FHeap) malloc(sizeof(FHeapNode));
    if (root == NULL) {
        printf(">>> Out of Memory: malloc a new FHeap <<<");
        exit(EXIT_FAILURE);
    }
    *root = NULL;
    return root;
}

/**
 * @brief free all the memory in FHeap
 * 
 * @param heap FHeap object
 */
public void FHeap_free(FHeap heap) {
    if (*heap != NULL) {
        FHeapNode current = *heap;
        FHeapNode end = current->siblings[0];
        end->siblings[1] = NULL;
        while (current != NULL) {
            if (current->child != NULL) {
                end->siblings[1] = current->child;  // end -> sub
                end = current->child->siblings[0];
                end->siblings[1] = NULL;
            }
            FHeapNode next = current->siblings[1];
            free(current);
            current = next;
        }
    }
    free(heap);
}

/**
 * @brief insert a key into FHeap
 * 
 * @param heap FHeap object
 * @param key 
 */
public void FHeap_insert(FHeap heap, ktype key) {
    // new node and setting
    FHeapNode newNode = (FHeapNode) malloc(sizeof(_FHeapNode));
    if (newNode == NULL) {
        printf(">>> Out of Memory: malloc a new FHeap node <<<");
        exit(EXIT_FAILURE);
    }
    newNode->key = key;
    newNode->degree = 0;
    newNode->childCutFlag = false;
    
    newNode->parent = NULL;
    newNode->child = NULL;

    // insert
    FHeapNode minRoot = *heap;
    if (minRoot == NULL) {  // is empty
        *heap = newNode;  // change root
        newNode->siblings[0] = newNode;
        newNode->siblings[1] = newNode;
        return;
    }
    // else not empty
    if (minRoot->key > newNode->key) {
        *heap = newNode;  // change root
    }
    __FHeap_linkNode(newNode, minRoot->siblings[1]);  // new <-> root1
    __FHeap_linkNode(minRoot, newNode);  // root <-> new
}

/**
 * @brief delete a key in FHeap
 * 
 * @param heap FHeap object
 * @param key 
 */
public void FHeap_delete(FHeap heap, ktype key) {
    if (*heap == NULL) {  // empty
        return;
    }
    if ((*heap)->key == key) {  // delete min
        FHeap_deleteMin(heap);
        return;
    }
    
    // find delete node
    FHeapNode deleteNode = __FHeap_findNode(heap, key);
    if (deleteNode == NULL) {  // not found
        // Is it suitable to perform min-tree joining?
        return;
    }

    // chain delete level
    if (deleteNode->parent != NULL) {  // not root level
        // set parent's child and chain
        if (deleteNode->parent->degree == 1) {  // one node
            deleteNode->parent->child = NULL;
        }
        else {
            __FHeap_linkNode(deleteNode->siblings[0], deleteNode->siblings[1]);  // delete0 <-> delete1
            deleteNode->parent->child = deleteNode->siblings[1];
        }
        // set degree and flag
        deleteNode->parent->degree--;
        if (deleteNode->parent->childCutFlag == true) {
            __FHeap_cascadingCut(heap, deleteNode->parent);
        }
        else {
            deleteNode->parent->childCutFlag = true;
        }
    }
    else {  // is root level
        if (deleteNode->siblings[1] == deleteNode) {  // one node
            *heap = NULL;  // become empty
        }
        else {
            // delete node will not be the smallest one in the root level
            __FHeap_linkNode(deleteNode->siblings[0], deleteNode->siblings[1]);  // delete0 <-> delete1
        }
    }

    // chain subtree level to root level
    if (deleteNode->child != NULL) {  // has child
        FHeapNode minChild = __FHeap_findMinNode(deleteNode->child, deleteNode->child);  // find subtree min
        // erase the parent in the subtree
        FHeapNode current = minChild;
        do {
            current->parent = NULL;
            current = current->siblings[1];
        } while (current != minChild);
        
        // chain
        if (*heap != NULL) {
            __FHeap_linkNode(minChild->siblings[0], (*heap)->siblings[1]);  // sub0 <-> root1
            __FHeap_linkNode(*heap, minChild);  // root <-> sub
        }
        if (*heap == NULL || minChild->key < (*heap)->key) {
            *heap = minChild;
        }
    }

    free(deleteNode);
    __FHeap_minTreeJoining(heap);
}

/**
 * @brief delete the minimum key in FHeap
 * 
 * @param heap FHeap object
 */
public void FHeap_deleteMin(FHeap heap) {
    if (*heap == NULL) {  // empty
        return;
    }

    FHeapNode deleteNode = *heap;

    // chain delete level
    if (deleteNode->siblings[1] == deleteNode) {  // one node
        *heap = NULL;
    }
    else {
        __FHeap_linkNode(deleteNode->siblings[0], deleteNode->siblings[1]);  // delete0 <-> delete1
        *heap = __FHeap_findMinNode(deleteNode->siblings[1], deleteNode->siblings[1]);  // find min node
    }

    // chain subtree level to root level
    if (deleteNode->child != NULL) {  // has child
        FHeapNode minChild = __FHeap_findMinNode(deleteNode->child, deleteNode->child);  // find subtree min
        // erase the parent in the subtree
        FHeapNode current = minChild;
        do {
            current->parent = NULL;
            current = current->siblings[1];
        } while (current != minChild);
        
        // chain
        if (*heap != NULL) {
            __FHeap_linkNode(minChild->siblings[0], (*heap)->siblings[1]);  // sub0 <-> root1
            __FHeap_linkNode(*heap, minChild);  // root <-> sub
        }
        if (*heap == NULL || minChild->key < (*heap)->key) {
            *heap = minChild;
        }
    }
    
    free(deleteNode);
    __FHeap_minTreeJoining(heap);
}

/**
 * @brief decrease a key in FHeap by an arbitrary value
 * 
 * @param heap FHeap object
 * @param key 
 * @param value the decreasing value
 */
public void FHeap_decreaseKey(FHeap heap, ktype key, ktype value) {
    if (*heap == NULL) {  // empty
        return;
    }
    
    // find decrease node and decrease it by value
    FHeapNode decreaseNode = __FHeap_findNode(heap, key);
    if (decreaseNode == NULL) {  // not found
        return;
    }
    decreaseNode->key -= value;

    // is root level or too small?
    if (decreaseNode->parent == NULL || decreaseNode->key >= decreaseNode->parent->key) {
        if (decreaseNode->key < (*heap)->key) {
            *heap = decreaseNode;
        }
        return;  // not thing happend
    }

    // else not root level
    // set parent's child and chain decrease level
    if (decreaseNode->parent->degree == 1) {  // one node
        decreaseNode->parent->child = NULL;
    }
    else {
        __FHeap_linkNode(decreaseNode->siblings[0], decreaseNode->siblings[1]);  // decrease0 <-> decrease1
        decreaseNode->parent->child = decreaseNode->siblings[1];
    }
    // set degree and flag
    decreaseNode->parent->degree--;
    if (decreaseNode->parent->childCutFlag == true) {
        __FHeap_cascadingCut(heap, decreaseNode->parent);
    }
    else {
        decreaseNode->parent->childCutFlag = true;
    }
    // chain decrease node to root level
    decreaseNode->parent = NULL;
    __FHeap_linkNode(decreaseNode, (*heap)->siblings[1]);  // decrease <-> root1
    __FHeap_linkNode(*heap, decreaseNode);  // root <-> decrease
    if (decreaseNode->key < (*heap)->key) {
        *heap = decreaseNode;
    }
}

/**
 * @brief print the FHeap
 * 
 * @param heap FHeap object
 */
public void FHeap_print(FHeap heap) {
    if (*heap == NULL) {
        return;
    }

    // find min degree and min key
    FHeapNode root = __FHeap_sortRootLevel(heap);
    FHeapNode rootCurrent = root;

    // use BFS to implement level-order traversal
    Queue queue = Queue_create();
    printf("============= %d =============\n", (*heap)->key);
    do {
        // print
        printf("%d:%d|%d \n", rootCurrent->key, rootCurrent->degree, rootCurrent->childCutFlag);
        // printf("%d ", rootCurrent->key);
        if (rootCurrent->child != NULL) {
            Queue_push(queue, rootCurrent->child);
            while (Queue_isEmpty(queue) == false) {
                FHeapNode head = Queue_pop(queue);  // pop a link of node in same level
                head = __FHeap_sortLevel(head, __FHeap_sortLevelKey_key);
                FHeapNode current = head;
                printf(" %d->[ ", current->parent->key);
                do {
                    printf("%d:%d|%d ", current->key, current->degree, current->childCutFlag);
                    // printf("%d ", current->key);
                    if (current->child != NULL) {
                        Queue_push(queue, current->child);
                    }
                    // next
                    current = current->siblings[1];
                } while (current != head);
                printf("] \n");
            }
        }
        // next
        rootCurrent = rootCurrent->siblings[1];
        printf("\n");
    } while (rootCurrent != root);
    Queue_free(queue);
}


/**
 * @brief use insertion sort to sort the node in the level
 * 
 * @param startNode the start node of the level
 * @param key determine which attribute to use for sorting
 * @return the new start node of the level 
 */
private FHeapNode __FHeap_sortLevel(FHeapNode startNode, int (*key)(FHeapNode, FHeapNode)) {
    FHeapNode current = startNode->siblings[1];
    while (current != startNode) {
        FHeapNode check = current->siblings[0];
        while (1) {
            if (key(check, current) <= 0) {  // find place
                break;
            }
            check = check->siblings[0];
            if (check->siblings[1] == startNode) {  // reach start
                startNode = current;
                break;
            }
        }
        if (check == current) {  // insert behind itself
            break;  // when the last node insert in front of the first node
        }
        FHeapNode temp = current;
        current = current->siblings[1];
        // chain
        __FHeap_linkNode(temp->siblings[0], temp->siblings[1]);  // temp0 <-> temp1
        __FHeap_linkNode(temp, check->siblings[1]);  // temp <-> check1
        __FHeap_linkNode(check, temp);  // check <-> temp
    }
    return startNode;
}

/**
 * @brief This function is used to decide the relation between 2 FHeapNode by thier key.
 * 
 * @param node1 
 * @param node2 
 * @return * If return value > 0, it will be seen as node1->key > node2->key. 
 * @return * If return value = 0, it will be seen as node1->key = node2->key.
 * @return * If return value < 0, it will be seen as node1->key < node2->key.
 */
private int __FHeap_sortLevelKey_key(FHeapNode node1, FHeapNode node2) {
    return node1->key - node2->key;
}

/**
 * @brief This function is used to decide the relation between 2 FHeapNode by thier degree.
 * 
 * @param node1 
 * @param node2 
 * @return * If return value > 0, it will be seen as node1->degree > node2->degree. 
 * @return * If return value = 0, it will be seen as node1->degree = node2->degree.
 * @return * If return value < 0, it will be seen as node1->degree < node2->degree.
 */
private int __FHeap_sortLevelKey_degree(FHeapNode node1, FHeapNode node2) {
    return node1->degree - node2->degree;
}

/**
 * @brief link node1 and node2 together. `node1` will be put at `left` side.
 * 
 * @param node1 
 * @param node2 
 */
private void __FHeap_linkNode(FHeapNode node1, FHeapNode node2) {
    node1->siblings[1] = node2;
    node2->siblings[0] = node1;
}


/**
 * @brief use insertion sort to sort root level
 * 
 * @param heap FHeap object
 * @return the first node
 */
private FHeapNode __FHeap_sortRootLevel(FHeap heap) {
    FHeapNode start = __FHeap_sortLevel(*heap, __FHeap_sortLevelKey_key);  // sort key
    start = __FHeap_sortLevel(start, __FHeap_sortLevelKey_degree);  // sort degree
    return start;
}

/**
 * @brief find min node in same linked list from start to end(not include) ([start, end))
 * 
 * @param start start finding node
 * @param end end finding node (this one will not be check)
 * @return min node 
 */
private FHeapNode __FHeap_findMinNode(FHeapNode start, FHeapNode end) {
    FHeapNode minNode = start;
    for (FHeapNode next = minNode->siblings[1]; next != end; next = next->siblings[1]) {
        if (next->key < minNode->key) {
            minNode = next;
        }
    }
    return minNode;
}

/**
 * @brief find a node by key
 * 
 * @param heap FHeap object
 * @param key indicate the node to find
 * @return node with key or NULL 
 */
private FHeapNode __FHeap_findNode(FHeap heap, ktype key) {
    // use BFS to find the node
    Queue queue = Queue_create();
    Queue_push(queue, *heap);
    while (Queue_isEmpty(queue) == false) {
        FHeapNode head = Queue_pop(queue);  // pop a link of node in same level
        FHeapNode current = head;
        do {
            if (current->key == key) {  // find
                Queue_free(queue);
                return current;
            }
            // we can ignore some node's child when the node is larger than key
            if (current->child != NULL && current->key < key) {
                Queue_push(queue, current->child);
            }
            // next
            current = current->siblings[1];
        } while (current != head);
    }
    Queue_free(queue);
    return NULL;
}

/**
 * @brief min-tree joining
 * 
 * @param heap FHeap object
 */
private void __FHeap_minTreeJoining(FHeap heap) {
    if (*heap == NULL) {  // empty
        return;
    }
    // use insertion sort
    FHeapNode current = __FHeap_sortRootLevel(heap);
    int degree = current->degree;
    while (1) {
        if (current->siblings[1]->degree == degree && current->degree == degree) {  // joining
            if (current == current->siblings[1]) {  // one node
                break;
            }
            // setting degree and flag
            FHeapNode bigNode = current->siblings[1];
            current->degree++;
            current->childCutFlag = false;
            bigNode->childCutFlag = false;
            
            // chain
            bigNode->parent = current;
            __FHeap_linkNode(current, bigNode->siblings[1]);  // current <-> bigNode1
            if (current->child == NULL) {
                current->child = bigNode;
                __FHeap_linkNode(bigNode, bigNode);  // bigNode <-> bigNode
            }
            else {
                __FHeap_linkNode(bigNode, current->child->siblings[1]);  // bigNode <-> child1
                __FHeap_linkNode(current->child, bigNode);  // child <-> bigNode
            }

            // next
            current = current->siblings[1];
        }
        else if (current->siblings[1]->degree > degree || current->degree > degree) {  // to next
            // current will be the smallest one in next degree
            FHeapNode start = __FHeap_sortRootLevel(heap);
            current = start;
            do {
                if(current->degree > degree) {
                    break;
                }
                current = current->siblings[1];
            } while (current != start);
            degree = current->degree;
        }
        else {  // to smaller one => end
            break;
        }
    }
}

/**
 * @brief cascading cut
 * 
 * @param heap FHeap object
 * @param startNode the node start to cascading cut
 */
private void __FHeap_cascadingCut(FHeap heap, FHeapNode startNode) {
    FHeapNode current = startNode;
    while (current->childCutFlag == true && current->parent != NULL) {
        // set parent's child and chain current level
        if (current->parent->degree == 1) {  // one node
            current->parent->child = NULL;
        }
        else {
            __FHeap_linkNode(current->siblings[0], current->siblings[1]);  // current0 <-> current1
            current->parent->child = current->siblings[1];
        }
        // set degree
        current->parent->degree--;

        // chain current node to root level
        FHeapNode tmpParent = current->parent;
        current->parent = NULL;
        __FHeap_linkNode(current, (*heap)->siblings[1]);  // current <-> root1
        __FHeap_linkNode(*heap, current);  // root <-> current
        if (current->key < (*heap)->key) {
            *heap = current;
        }
        
        // next
        current = tmpParent;
    }
    current->childCutFlag = true;
}



// =============================== define the method of Queue =============================== //
/**
 * @brief create a Queue
 * 
 * @return Queue object
 */
public Queue Queue_create(void) {
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
 * @brief free all the memory in Queue
 * 
 * @param object Queue object
 */
public void Queue_free(Queue queue) {
    Node curtent = queue->head;
    Node temp = NULL;
    while (curtent != NULL) {
        temp = curtent;
        curtent = temp->next;
        free(temp);
    }
    free(queue);
}

/**
 * @brief push the value into the Queue
 * 
 * @param queue Queue object
 * @param value 
 */
public void Queue_push(Queue queue, void* value) {
    Node new = (Node) malloc(sizeof(_Node));
    if (new == NULL) {
        printf(">>> Out of Memory: malloc a new node of the Queue <<<");
        exit(EXIT_FAILURE);
    }

    new->value = value;
    new->next = NULL;  // push to the end of the queue

    if (queue->head == NULL) {  // Queue is empty
        queue->head = new;
    }
    else {
        queue->tail->next = new;  // tail links to new
    }
    queue->tail = new;
}

/**
 * @brief pop out the top element in the Queue
 * 
 * @param queue Queue object
 * @return the value of the top element
 * @warning if the Queue is empty, it will not work correctly
 */
public void* Queue_pop(Queue queue) {
    Node out = queue->head;
    void *value = out->value;

    queue->head = out->next;
    if (queue->head == NULL) {  // Queue will be empty
        queue->tail = NULL;
    }

    free(out);
    return value;
}

/**
 * @brief check whether the Queue is empty
 * 
 * @param queue Queue object
 * @return true if the Queue is empty else false
 */
public bool Queue_isEmpty(Queue queue) {
    return queue->head == NULL;  // queue->tail == NULL
}

/**
 * @brief print the Queue
 * 
 * @param queue Queue object
 */
public void Queue_print(Queue queue) {
    printf("<%p>: [pop] <- ", queue);

    Node curtent = queue->head;
    while (curtent != NULL) {
        printf("%p <- ", curtent->value);
        curtent = curtent->next;
    }
    
    printf("[push]\n");
}
