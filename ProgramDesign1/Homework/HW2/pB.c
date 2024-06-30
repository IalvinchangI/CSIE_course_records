#include<stdio.h>

int main(void){
	int k1, k2, k3;
	long long int n;

	scanf("%lld %d %d %d", &n, &k1, &k2, &k3);
	printf("%lld", (k1 + k2 + k3 - k1 / 3 -k2 / 3 - k3 / 3) * n);
	return 0;
}
