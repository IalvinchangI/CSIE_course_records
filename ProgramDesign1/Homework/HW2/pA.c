#include<stdio.h>

int main(void){
	long long a1, a2, b1, b2;
	
	scanf("%lld/%lld+%lld/%lld", &a1, &b1, &a2, &b2);
	printf("%lld/%lld", a1 * b2 + a2 * b1, b1 * b2);
	return 0;
}
