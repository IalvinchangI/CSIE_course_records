#include<stdio.h>

int main(void){
	int a, b, c;
	int sum = 0;
	scanf("%d-%d-%d", &a, &b, &c);
	
	if (a == b && b == c) {
		sum += 300;
	}
	if (a + b < c) {
		sum += 150;
	}
	if (a == c) {
		sum += 100;
	}
	if (a < b) {
		sum += 50;
	}
	
	printf("%d", sum);

	return 0;
}
