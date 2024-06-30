#include<stdlib.h>
#include<string.h>
#include<stdio.h>

struct LinkedList;

struct LinkedList {
    char name[20];
    int passwd_public[10][10][10][10];
    int ***passwd_private;
    struct LinkedList *back_head;
    struct LinkedList *next;
};

struct LinkedList* create(char target[20], int p1, int n, int m, int l, int p2) {
    struct LinkedList *node = malloc(sizeof(struct LinkedList));
    if (node == NULL) exit(1);  // failed
    strcpy(node -> name, target);  // set name
    // set passwd_public
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                for (int l = 0; l < 10; l++) {
                    (node -> passwd_public)[i][j][k][l] = p1;
                }
            }
        }
    }
    // set passwd_private
    node -> passwd_private = malloc(sizeof(int**) * n);
    if (node -> passwd_private == NULL) exit(1);  // failed
    for (int i = 0; i < n; i++) {
        (node -> passwd_private)[i] = malloc(sizeof(int*) * m);
        if ((node -> passwd_private)[i] == NULL) exit(1);  // failed
        for (int j = 0; j < m; j++) {
            (node -> passwd_private)[i][j] = malloc(sizeof(int) * l);
            if ((node -> passwd_private)[i][j] == NULL) exit(1);  // failed
            for (int k = 0; k < l; k++) {
                (node -> passwd_private)[i][j][k] = p2;
            }
        }
    }
    // set pointer
    node -> back_head = NULL;
    node -> next = NULL;
    return node;
}
