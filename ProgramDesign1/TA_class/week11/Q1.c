#include <stdio.h>

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

int main() {
    int a,b;
    
    scanf("%d %d", &a, &b);
    
    printf("before:\n");
    printf("%d %d\n", a, b);
    
    swap(&a, &b);
    
    printf("after:\n");
    printf("%d %d\n", a, b);
}

