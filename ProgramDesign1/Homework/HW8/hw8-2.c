void modify(int* a,int val) {
    *a = val - 1;
}

int query(int (*mp)[131][131][131],char a[4],char b[4]) {
    return ((*mp)[a[0]][a[1]][a[2]] + (*mp)[b[0]][b[1]][b[2]]) & 1;
}