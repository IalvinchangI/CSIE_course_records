#include<stdlib.h>
#include<stdio.h>
#include<math.h>


typedef struct __node {
    int value;
    int shortest;
    struct __node *child[2];
} _Node, *LeftistTree;


LeftistTree insert(LeftistTree tree, int value);
LeftistTree deleteMin(LeftistTree tree);
LeftistTree meld(LeftistTree tree1, LeftistTree tree2);

int main(void) {
    LeftistTree tree = NULL;

    tree = insert(tree, 3);
    tree = insert(tree, 30);
    tree = insert(tree, 15);
    tree = insert(tree, 4);
    tree = insert(tree, 7);
    tree = insert(tree, 18);
    tree = deleteMin(tree);
    tree = deleteMin(tree);

    return 0;
}


LeftistTree insert(LeftistTree tree, int value) {
    // init new tree
    LeftistTree valueTree = (LeftistTree) malloc(sizeof(_Node));
    valueTree->value = value;
    valueTree->shortest = 1;
    valueTree->child[0] = NULL;
    valueTree->child[1] = NULL;

    return meld(tree, valueTree);
}

LeftistTree deleteMin(LeftistTree tree) {
    if (tree == NULL) {
        printf("Tree is empty.\n");
        return NULL;
    }

    // get subtree
    LeftistTree tree0 = tree->child[0];
    LeftistTree tree1 = tree->child[1];

    return meld(tree0, tree1);
}

LeftistTree meld(LeftistTree tree1, LeftistTree tree2) {
    // stop recursive and ensure tree1, tree2 after this section won't be NULL
    if (tree1 == NULL && tree2 == NULL) {  // for first case
        return NULL;
    }
    if (tree1 == NULL) {  // for recursive case and first case
        return tree2;
    }
    if (tree2 == NULL) {  // for first case
        return tree1;
    }

    // find the smallest root value of the tree
    LeftistTree subRoot = NULL;
    LeftistTree subRightTree = NULL;
    if (tree1->value <= tree2->value) {  // define a min tree
        subRoot = tree1;
        subRightTree = tree2;
    }
    else {
        subRoot = tree2;
        subRightTree = tree1;
    }
    
    // recursive
    subRightTree = meld(subRoot->child[1], subRightTree);  // child[1] may be NULL, but subRightTree is not NULL. The result won't be NULL.
    LeftistTree subLeftTree = subRoot->child[0];  // child[0] may be NULL

    // rotate
    int leftShort = (subLeftTree == NULL) ? 0 : subLeftTree->shortest;
    if (leftShort < subRightTree->shortest) {
        subRoot->child[0] = subRightTree;
        subRoot->child[1] = subLeftTree;
    }
    else {
        subRoot->child[0] = subLeftTree;
        subRoot->child[1] = subRightTree;
    }

    // calculate and update subRoot's shortest (base on height)
    subRoot->shortest = ((leftShort < subRightTree->shortest) ? leftShort : subRightTree->shortest) + 1;
    // next
    return subRoot;
}
