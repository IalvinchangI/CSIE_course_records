#include <stdio.h>

int main()
{
    printf("Please input two integer and get their GCD(greastest common divisor).\n");
    int n, m, gcd;
    scanf("%d%d", &n, &m);

    while (n != m) {
	    if (n > m) {
		    n -= m;
	    } else {
		    m -= n;
	    }
    }
    gcd = m;
    
    printf("The GCD of the two numbers is %d.\n", gcd);
    return 0;
}
