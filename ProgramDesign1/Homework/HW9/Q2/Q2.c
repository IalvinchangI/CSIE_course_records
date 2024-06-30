#include<stdio.h>
#include"decimal2binary.h"

int main(void) {
	float f1 = 1.175494350822287507968736537222245677818665556772087521508751706278417259454727172851560500000000000000000000000000000000e-38f;
	float f2 = 1.175494350822287500e-38f;
	float f3 = 1.175494300e-38f;
	float f4 = 1.17549400e-38f;
	float f5 = 1.1754900e-38f;
	double d1 = 1.175494350822287507968736537222245677818665556772087521508751706278417259454727172851560500000000000000000000000000000000e-38f;
	double d2 = 1.175494350822287500e-38f;
	
	// ========== 1 ========== //
	printf("========== 1 ==========\n");
	printf("float: \n");
	printf("\tf1: %.200g:\n\t    ", f1);
	decimal2binary_pointer(f1);
	printf("\tf1/2: %.200g:\n\t    ", f1 / 2);
	decimal2binary_pointer(f1 / 2);
	printf("\tf1/4: %.200g:\n\t    ", f1 / 4);
	decimal2binary_pointer(f1 / 4);
	printf("\tf1/8: %.200g:\n\t    ", f1 / 8);
	decimal2binary_pointer(f1 / 8);
	unsigned int ui = 1;
	float fs = *(float*)&ui;
	printf("\tfs: %.200g:\n\t    ", fs);
	decimal2binary_pointer(fs);
	printf("\tfs / 2: %.200g:\n\t    ", fs / 2);
	decimal2binary_pointer(fs / 2);

	printf("\ndouble: \n");
	printf("\td1: %.200g:\n\t    ", d1);
	decimal2binary_pointer__d(d1);
	//unsigned long long ulli = 1ULL << 51;
	unsigned long long ulli = 1;
	double ds = *(double*)&ulli;
	printf("\tds: %.200g:\n\t    ", ds);
	decimal2binary_pointer__d(ds);

	// ========== 2 ========== //
	printf("\n========== 2 ==========\n");
	printf("float:  %.200g:\n\t", 0.0f);
	decimal2binary_pointer(0.0f);
	printf("double: %.200g:\n\t", 0.0);
	decimal2binary_pointer__d(0.0);

	// ========== 3 ========== //
	printf("\n========== 3 ==========\n");
	if (f1 == f2) { printf("%.100e = %.100e", f1, f2); } else { printf("%.100e != %.100e", f1, f2); }
	printf("\n");
	if (f1 == f3) { printf("%.100e = %.100e", f1, f3); } else { printf("%.100e != %.100e", f1, f3); }
	printf("\n");
	if (f1 == f4) { printf("%.100e = %.100e", f1, f4); } else { printf("%.100e != %.100e", f1, f4); }
	printf("\n");
	if (f1 == f5) { printf("%.100e = %.100e", f1, f5); } else { printf("%.100e != %.100e", f1, f5); }
	printf("\n\n");
	printf("float: \n");
	printf("\tf1: %.200g:\n\t    ", f1);
	decimal2binary_pointer(f1);
	printf("\tf2: %.200g:\n\t    ", f2);
	decimal2binary_pointer(f2);

	printf("\n\n");
	if(d1 == d2) {
		printf("%.100e = \n%.100e", d1, d2);
	} else {
		printf("%.100e != \n%.100e", d1, d2);
	}
	printf("\n");
	printf("\ndouble: \n");
	printf("\td1: %.200g:\n\t    ", d1);
	decimal2binary_pointer__d(d1);
	printf("\td2: %.200g:\n\t    ", d2);
	decimal2binary_pointer__d(d2);
	
	return 0;
}

