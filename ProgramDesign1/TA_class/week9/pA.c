#include<stdio.h>

int f(int);
int y(int);
int z(int);

int main(void) {
	int x = 0;
	scanf("%d", &x);
	printf("%d", f(x));
	return 0;
}

int f(int x) {
	return y(z(x)) + 7 * x;
}

int y(int x) {
	return 6 * x + 10;
}

int z(int x) {
	return 2023 - 5 * x;
}
