#include<stdio.h>

int main(void){
    int input;
    scanf("%d", &input);
    
    int temp = input % 100;
    int ten = temp / 10;  // origin tens digit -> new units digit
    int unit = temp % 10;  // origin units digit -> new tens digit

    int final = (input - temp) + unit * 10 + ten;
    printf("%d", final * 2023);
    return 0;
}