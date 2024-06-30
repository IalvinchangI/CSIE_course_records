#include<stdio.h>

// ===== decimal2binary ===== //
// print
void print_bit_pattern(unsigned int);  // print the bit pattern of unsigned int
void print_bit_pattern__d(unsigned long long);  // print the bit pattern of unsigned long long

// input: float
void decimal2binary_pointer(float);  // an integer pointer to float or double
void decimal2binary_union(float);  // union
void decimal2binary_field(float);  // bit field

// input: double
void decimal2binary_pointer__d(double);  // an integer pointer to float or double
void decimal2binary_union__d(double);  // union
void decimal2binary_field__d(double);  // bit field


// ===== binary2decimal ===== //
// scan
unsigned int binary2decimal(char *);  // convert 32 bits to unsigned int and return it
unsigned long long binary2decimal__d(char *);  // convert 64 bits to unsigned long long and return it

// output: float
void binary2decimal_pointer(char *);  // an integer pointer to float or double
void binary2decimal_union(char *);  // union
void binary2decimal_field(char *);  // bit field

// output: double
void binary2decimal_pointer__d(char *);  // an integer pointer to float or double
void binary2decimal_union__d(char *);  // union
void binary2decimal_field__d(char *);  // bit field



// for bit field
typedef struct {
	unsigned char a1 : 1;
	unsigned char a2 : 1;
	unsigned char a3 : 1;
	unsigned char a4 : 1;
	unsigned char a5 : 1;
	unsigned char a6 : 1;
	unsigned char a7 : 1;
	unsigned char a8 : 1;
} bit;  // read & write each bit in 1 byte




int main(void) {
	printf("===== decimal to binary =====\n");
	float f = 0.f;
	double d = 0.;
	// input decimal number
	printf("float: ");
	scanf("%f", &f);
	printf("double: ");
	scanf("%lf", &d);
	
	// output bit pattern
	printf("\n=== float ===\n");
	decimal2binary_pointer(f);
	decimal2binary_union(f);
	decimal2binary_field(f);

	printf("\n=== double ===\n");
	decimal2binary_pointer__d(d);
	decimal2binary_union__d(d);
	decimal2binary_field__d(d);
	
	
	
	printf("\n\n===== binary to decimal =====\n");
	char f_b[33] = {0};
	char d_b[65] = {0};
	// input bit pattern
	printf("float: ");
	scanf("%s", f_b);
	printf("double: ");
	scanf("%s", d_b);
	
	// output decimal number
	printf("\n=== float ===\n");
	binary2decimal_pointer(f_b);
	binary2decimal_union(f_b);
	binary2decimal_field(f_b);

	printf("\n=== double ===\n");
	binary2decimal_pointer__d(d_b);
	binary2decimal_union__d(d_b);
	binary2decimal_field__d(d_b);



	return 0;
}





// ===== decimal2binary ===== //

void print_bit_pattern(unsigned int decimal) {  // print the bit pattern of unsigned int
	for (unsigned int i = 1 << 31; i > 0; i = i >> 1) {
		printf("%d", (decimal & i) ? 1 : 0);
	}
	printf("\n");
}

void print_bit_pattern__d(unsigned long long decimal) {  // print the bit pattern of unsigned long long
	for (unsigned long long i = 1ULL << 63; i > 0; i = i >> 1) {
		printf("%d", (decimal & i) ? 1 : 0);
	}
	printf("\n");
}



// =========================== float =============================

void decimal2binary_pointer(float f) {  // an integer pointer to float or double
	unsigned int *pointer = (unsigned int*)&f;
	printf("pointer: \t");
	print_bit_pattern(*pointer);
}

void decimal2binary_union(float f) {  // union
	union {
		float f;
		unsigned int i;
	} f2i = {.f = f};
	printf("union: \t\t");
	print_bit_pattern(f2i.i);
}

void decimal2binary_field(float f) {  // bit field
	printf("bit field: \t");
	for (int i = 3; i >= 0; i--) {
		bit byte = *((bit*)&f + i);
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



// =========================== double =============================

void decimal2binary_pointer__d(double d) {  // an integer pointer to float or double
	unsigned long long *pointer = (unsigned long long*)&d;
	printf("pointer: \t");
	print_bit_pattern__d(*pointer);
}

void decimal2binary_union__d(double d) {  // union
	union {
		double d;
		unsigned long long i;
	} d2i = {.d = d};
	printf("union: \t\t");
	print_bit_pattern__d(d2i.i);
}

void decimal2binary_field__d(double d) {  // bit field
	printf("bit field: \t");
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







// ===== binary2decimal ===== //

unsigned int binary2decimal(char *s) {  // convert 32 bits to unsigned int and return it
	unsigned int ui = 0;
	for (int i = 0; i < 32; i++) {
		if (s[i] != '0') {
			ui |= 1 << (31 - i);
		}
	}
	return ui;
}

unsigned long long binary2decimal__d(char *s) {  // convert 64 bits to unsigned long long and return it
	unsigned long long ui = 0;
	for (int i = 0; i < 63; i++) {
		if (s[i] != '0') {
			ui |= 1ULL << (63 - i);
		}
	}
	return ui;
}



// =========================== float =============================

void binary2decimal_pointer(char *s) {  // an integer pointer to float or double
	unsigned int ui = binary2decimal(s);
	printf("pointer: \t%.200g\n", *(float*)&ui);
}

void binary2decimal_union(char *s) {  // union
	union {
		unsigned int i;
		float f;
	} i2f = {.i = binary2decimal(s)};
	printf("union: \t\t%.200g\n", i2f.f);
}

void binary2decimal_field(char *s) {  // bit field
	float out = 0.f;
	for (int i = 3, j = 0; i >= 0; i--, j += 8) {
		bit b = {
			.a1 = s[j + 7] - '0',
			.a2 = s[j + 6] - '0',
			.a3 = s[j + 5] - '0',
			.a4 = s[j + 4] - '0',
			.a5 = s[j + 3] - '0',
			.a6 = s[j + 2] - '0',
			.a7 = s[j + 1] - '0',
			.a8 = s[j + 0] - '0'
		};
		*((bit*)&out + i) = b;
	}
	printf("bit field: \t%.200g\n", out);
}



// =========================== double =============================

void binary2decimal_pointer__d(char *s) {  // an integer pointer to float or double
	unsigned long long ui = binary2decimal__d(s);
	printf("pointer: \t%.200g\n", *(double*)&ui);
}

void binary2decimal_union__d(char *s) {  // union
	union {
		unsigned long long i;
		double f;
	} i2f = {.i = binary2decimal__d(s)};
	printf("union: \t\t%.200g\n", i2f.f);
}

void binary2decimal_field__d(char *s) {  // bit field
	double out = 0.f;
	for (int i = 7, j = 0; i >= 0; i--, j += 8) {
		bit b = {
			.a1 = s[j + 7] - '0',
			.a2 = s[j + 6] - '0',
			.a3 = s[j + 5] - '0',
			.a4 = s[j + 4] - '0',
			.a5 = s[j + 3] - '0',
			.a6 = s[j + 2] - '0',
			.a7 = s[j + 1] - '0',
			.a8 = s[j + 0] - '0'
		};
		*((bit*)&out + i) = b;
	}
	printf("bit field: \t%.200g\n", out);
}
