#include<stdio.h>

int main(void){
	int a, b, N;
	
	scanf("%d,%d,%d", &a, &b, &N);
	printf("%d", (b << a) + (a | b) - (N >> b) * (a ^ b) - (a & b));
	return 0;
}
