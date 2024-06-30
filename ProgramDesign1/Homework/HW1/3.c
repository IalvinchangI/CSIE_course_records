#include<stdio.h>
#include<math.h>

int main(void){
    int a, b, c;
    scanf("%d %d %d", &a, &b, &c);

    double raw = sqrt(a * c) + pow(a, b) + log10(a * b * c);  // calculate formula
    double final = ((int)(raw * 1000 + 0.5)) * 0.001;    // round the number to the third decimal place
    
    printf("%.3f", final);
    return 0;
}