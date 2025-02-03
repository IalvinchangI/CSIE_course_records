#include<stdio.h>

#define compare_creator(type, name, a1, b1, a2, b2) type compare_##type##name(void *a1, void *b1) {return *(int*)a2 - *(int*)b2;}


void print_array(int i, int *array) {
    for (int x = 0; x < i; x++) {
        printf("%d ", *(array + x));
    }
    printf("\n");
}

void swap(void *a, void *b, int size) {
    for (int i = 0; i < size; i++) {
        int temp = *((char*)a + i);
        *((char*)a + i) = *((char*)b + i);
        *((char*)b + i) = temp;
    }
}

compare_creator(int, , a, b, a, b)  // small --> big
compare_creator(int, SE, a, b, b, a)  // big --> small
// compare_creator(float, , a, b, a, b)  // small --> big

void sort_quick(void *array, int length, int size, int (*compare)(void*, void*)) {
    // general quick sort  // compare return delta(l - r) between 2 input
    if (length <= 1) {  // stop
        return;
    }
    // sort
    char *end = (char*)array + (length - 1) * size;
    char *left = (char*)array + size;
    char *right = end;

    while (1) {
        while (left < end) {
            if (compare(left, array) >= 0) break;  // >=0 : 放right
            left += size;
        }
        while (right > (char*)array) {
            if (compare(right, array) < 0) break;  // <0 : 放left
            right -= size;
        }
        if (left >= right) break;  // not swap
        swap(left, right, size);
    }
    // left 最後指的東西一定比標準大
    // right 最後指的東西一定比標準小
    swap(array, right, size);  // 讓right指向標準

    // recursion
    int left_length = ((int)left - (int)array) / size;
    sort_quick(array, left_length, size, compare);  // left
    sort_quick(left, length - left_length, size, compare);  // right
}

int main(void) {
    int array[10] = {70, 453, 47, 2, 2, 98, 20, 45, 92, 76};
    sort_quick(array, 10, sizeof(int), compare_int);
    print_array(10, array);
    sort_quick(array, 10, sizeof(int), compare_intSE);
    print_array(10, array);
}

