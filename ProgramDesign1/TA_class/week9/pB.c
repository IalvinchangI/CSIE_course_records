#include<stdio.h>

int pow(int, int);

int main(void) {
	int a = 0, n = 0;
	scanf("%d %d", &a, &n);
	printf("%d", pow(a, n));
	return 0;
}

int pow(int a, int n) {
	if (n == 1) {
		return a;
	}
	return pow(a, n - 1) * a;
}
