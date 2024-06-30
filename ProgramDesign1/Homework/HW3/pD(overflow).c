#include <stdio.h>

int main()
{
    long long a, b;
    long long delta;
    scanf("%lld%lld", &a, &b);
    
    if (a >= 0) {
	    if (b > 0 && b > 9223372036854775807 - a) {
		    printf("Yes");
	    }
	    else {
		    printf("No");
	    }
    }
    else { 
	    if (b < 0 && b < -9223372036854775807 - 1 - a) {
		    printf("Yes");
	    }
	    else {
		    printf("No");
	    }
    }
    

    
    return 0;
}
