#include<stdlib.h>

void init(int*** mp) {
    // 'A' = 65, 'Z' = 90
    for (int i = 65; i <= 90; i++) {
        mp[i] = (int**)malloc(sizeof(int*) * 26) - 65;
        for (int j = 65; j <= 90; j++) {
            mp[i][j] = (int*)malloc(sizeof(int) * 26) - 65;
            for (int h = 65; h <= 90; h++) {
                mp[i][j][h] = 1;
            }
        }
    }
}

void modify(int* a,int val) {
    *a = val;
}

int query(int**** mp,char a[4],char b[4]) {
    return ((*mp)[a[0]][a[1]][a[2]] + (*mp)[b[0]][b[1]][b[2]]) & 1;
}