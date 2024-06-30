#include <stdio.h>
#include <math.h>

int main()
{
    float a,b,c;
    scanf("(%f,%f,%f)",&a,&b,&c);
    
    float s = ( a + b + c ) / 2;
    double ans = sqrt((double) s*(s-a)*(s-b)*(s-c));
     
    printf("Area of triangle (%d,%d,%d) is %d.\n", (int)a, (int)b, (int)c, (int)ans);
    return 0;
}
