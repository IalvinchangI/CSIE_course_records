/*
 * File:   testbench.c
 * Author: alvin
 *
 * Created on 2025?10?18?, ?? 3:59
 */


#include <xc.h>
#define a 256
#define b 32767

extern long test_function(int x, int y);
//extern unsigned long unsigned_test_function(unsigned int x, unsigned int y);

void main(void) {
    volatile long ans = test_function((int)a, (int)b);
//    volatile long ans = unsigned_test_function((unsigned int)a, (unsigned int)b);
    
    while(1);
    return;
}
