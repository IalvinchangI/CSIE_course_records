#include<stdio.h>

int main(void){
	long long a, b, c, d, e;
	char skip;

	scanf("NCKU%lld%c%lld%c%lld%c%lld%c%lld", &a, &skip, &b, &skip, &c, &skip, &d, &skip, &e);
	printf("%lld    %lld    %lld    %lld    %lld", a, b, c, d, e);
	return 0;
}
