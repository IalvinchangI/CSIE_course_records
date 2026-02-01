/*
 * File:   advance_main.c
 * Author: alvin
 *
 * Created on 2025?10?14?, ?? 12:54
 */


#include <xc.h>
#pragma config WDT = OFF

extern unsigned int count_primes(unsigned int a, unsigned int b);

void main(void){
    volatile unsigned int ans = count_primes(35677, 65521);
    while(1);
    return;
}
