#include<stdio.h>
#include<stdlib.h>

typedef double (*F)(double, int);  // new type for function pointer

double power(double, int);  // x ^ n
double multiply(double, int);  // x * n
double divide(double, int);  // x / n
double powerpower(F, double, int, int);  // (x ? n) ^ m



int main(int argc, char* argv[]) {
	// get value
	double x = atof(argv[1]);
	int n = atoi(argv[2]);
	int m = atoi(argv[3]);
	
	// output
	printf("passing power into powerpower: %.200g\n", powerpower(power, x, n, m));
	printf("passing multiply into powerpower: %.200g\n", powerpower(multiply, x, n, m));
	printf("passing divide into powerpower: %.200g\n", powerpower(divide, x, n, m));

	return 0;
}



double power(double x, int n) {  // x ^ n
	double result = 1;
	if (n >= 0) for (int i = 0; i < n; result *= x, i++);
	else if (n < 0) for (int i = 0; i > n; result /= x, i--);
	return result;
}

double multiply(double x, int n) {  // x * n
	return x * n;
}

double divide(double x, int n) {  // x / n
	return x / n;
}

double powerpower(F operation, double x, int n, int m) {  // (x operation n) ^ m
	return power(operation(x, n), m);
}

