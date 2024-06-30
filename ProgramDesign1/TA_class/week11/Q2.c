#include <stdio.h>

void QSORT(int *nums, int left, int right){  
    int len = right - left;
    
    if (len <= 0) {  // stop
	    return;
    }
    
    int point_left = left, point_right = right;  // index
    int standard = nums[left];
    nums[point_left] = 0;

    while (point_left != point_right) {
	    if (nums[point_left] == 0) {
		    if (nums[point_right] >= standard) {
			    point_right--;
		    } else {
			    nums[point_left] = nums[point_right];
			    nums[point_right] = 0;
			    point_left++;
		    }
	    } else {
		    if (nums[point_left] < standard) {
			    point_left++;
		    } else {
			    nums[point_right] = nums[point_left];
			    nums[point_left] = 0;
			    point_right--;
		    }
	    }
    }
    // point_left == point_right
    nums[point_left] = standard;
    
    QSORT(nums, left, point_left - 1);
    QSORT(nums, point_left + 1, right);
}

int main(void){
    
    int array[10];
    int len = sizeof(array) / sizeof(int);
    
    for(int i = 0; i < len; i++){
        scanf("%d",&array[i]);
    }
    
    QSORT(array, 0, len - 1);

    for(int i=0; i<10; i++){
        printf("%d ",array[i]);
    }
}
