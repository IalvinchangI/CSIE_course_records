#include<stdio.h>

int main(void) {
	int n;
	scanf("%d", &n);
	
	double e = 1., deno = 1;
	for (int i = 1; i <= n; e += deno, i++, deno *= 1. / i);
	
	printf("%.30f", e);

	return 0;
}
