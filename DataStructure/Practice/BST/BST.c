#include<stdlib.h>
#include<stdio.h>


typedef struct __node {
    int value;
    struct __node *child[2];
} _Node, *BST;


BST insert(BST tree, int value);
BST delete(BST tree, int value);


int main(void) {
    BST tree = NULL;

    tree = insert(tree, 20);
    tree = insert(tree, 40);
    tree = insert(tree, 30);
    tree = insert(tree, 10);
    tree = insert(tree, 25);
    tree = insert(tree, 15);
    tree = insert(tree, 6);
    tree = insert(tree, 2);
    tree = insert(tree, 8);
    tree = insert(tree, 7);
    tree = insert(tree, 18);
    tree = insert(tree, 35);
    tree = delete(tree, 150);


    return 0;
}

BST insert(BST tree, int value) {
    // new node
    BST node = (BST) malloc(sizeof(_Node));
    node->value = value;
    node->child[0] = NULL;
    node->child[1] = NULL;

    // empty tree?
    if (tree == NULL) {
        return node;
    }

    // insert
    BST current = tree;
    BST parent = NULL;
    while (current != NULL) {
        parent = current;
        current = current->child[current->value <= value];
    }
    parent->child[parent->value <= value] = node;

    return tree;
}

BST delete(BST tree, int value) {
    // empty tree?
    if (tree == NULL) {
        return NULL;
    }

    // find
    BST current = tree;
    BST parent = NULL;
    while (current != NULL && current->value != value) {
        parent = current;
        current = current->child[current->value < value];
    }
    BST deleteNode = current;
    if (deleteNode == NULL) {
        printf("Not Found\n");
        return tree;
    }

    // delete
    if (deleteNode->child[0] == NULL && deleteNode->child[1] == NULL) {  // degree 0
        free(deleteNode);
        if (parent == NULL) {
            return NULL;
        }
        parent->child[parent->value <= value] = NULL;
        return tree;
    }
    if (deleteNode->child[0] == NULL || deleteNode->child[1] == NULL) {  // degree 1
        BST child = (deleteNode->child[1] == NULL) ? deleteNode->child[0] : deleteNode->child[1];
        free(deleteNode);
        if (parent == NULL) {
            return child;
        }
        parent->child[parent->value <= value] = child;
        return tree;
    }
    // degree 2
    current = deleteNode->child[1];
    BST subParent = deleteNode;
    while (current->child[0] != NULL) {  // find max's min
        subParent = current;
        current = current->child[0];
    }
    deleteNode->value = current->value;
    subParent->child[subParent == deleteNode] = current->child[1];
    free(current);

    return tree;
}

