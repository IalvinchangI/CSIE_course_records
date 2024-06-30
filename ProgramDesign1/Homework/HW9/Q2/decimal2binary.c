#include<stdio.h>

void print_bit_pattern(unsigned int);
void print_bit_pattern__d(unsigned long long);

void decimal2binary_pointer(float);
void decimal2binary_union(float);
void decimal2binary_field(float);

void decimal2binary_pointer__d(double);
void decimal2binary_union__d(double);
void decimal2binary_field__d(double);

typedef struct {
	unsigned char a1 : 1;
	unsigned char a2 : 1;
	unsigned char a3 : 1;
	unsigned char a4 : 1;
	unsigned char a5 : 1;
	unsigned char a6 : 1;
	unsigned char a7 : 1;
	unsigned char a8 : 1;
} bit;


void print_bit_pattern(unsigned int decimal) {
	for (unsigned int i = 1 << 31; i > 0; i = i >> 1) {
		printf("%d", (decimal & i) ? 1 : 0);
	}
	printf("\n");
}

void print_bit_pattern__d(unsigned long long decimal) {
	for (unsigned long long i = 1ULL << 63; i > 0; i = i >> 1) {
		printf("%d", (decimal & i) ? 1 : 0);
	}
	printf("\n");
}


// ======================= float =============================

void decimal2binary_pointer(float f) {
	unsigned int *pointer = (unsigned int*)&f;
	print_bit_pattern(*pointer);
}

void decimal2binary_union(float f) {
	union {
		float f;
		unsigned int i;
	} f2i = {.f = f};
	print_bit_pattern(f2i.i);
}

void decimal2binary_field(float f) {
	// printf("\n%p\n", &f);
	for (int i = 3; i >= 0; i--) {
		bit byte = *((bit*)&f + i);
		// printf("%p\t", (bit*)&f + i);
		printf("%d", byte.a8);
		printf("%d", byte.a7);
		printf("%d", byte.a6);
		printf("%d", byte.a5);
		printf("%d", byte.a4);
		printf("%d", byte.a3);
		printf("%d", byte.a2);
		printf("%d", byte.a1);
		// printf("\t%d\n", *((unsigned char*)&byte));
	}
	printf("\n");
}


// =========================== double =============================

void decimal2binary_pointer__d(double d) {
	unsigned long long *pointer = (unsigned long long*)&d;
	print_bit_pattern__d(*pointer);
}

void decimal2binary_union__d(double d) {
	union {
		double d;
		unsigned long long i;
	} d2i = {.d = d};
	print_bit_pattern__d(d2i.i);
}

void decimal2binary_field__d(double d) {
	for (int i = 7; i >= 0; i--) {
		bit byte = *((bit*)&d + i);
		printf("%d", byte.a8);
		printf("%d", byte.a7);
		printf("%d", byte.a6);
		printf("%d", byte.a5);
		printf("%d", byte.a4);
		printf("%d", byte.a3);
		printf("%d", byte.a2);
		printf("%d", byte.a1);
	}
	printf("\n");
}
