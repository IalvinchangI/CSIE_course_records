#include <stdio.h>

void decimalToBinary(int num) {   
    if (num == 0) {
        printf("0");
        return;
    }
    int m = 1 << 30;
    int TF = 0;
    int binary = 0;
    for (int i = 30; i >= 0; i--, m = m >> 1) {
	    binary = num & m;
	    if (binary == 0 && TF == 0) {
		    continue;
	    }
	    TF = 1;
	    printf(binary ? "1" : "0");
    }
}

int main() {
    int num;
    
    printf("Enter a decimal number: ");
    scanf("%d", &num);

    decimalToBinary(num);

    return 0;
}
