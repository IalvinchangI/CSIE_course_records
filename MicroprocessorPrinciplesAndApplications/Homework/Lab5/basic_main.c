/*
 * File:   basic_main.c
 * Author: alvin
 *
 * Created on 2025?10?14?, ?? 12:20
 */


#include <xc.h>

extern unsigned int is_prime(unsigned char n);

void main(void){
    volatile unsigned char ans = is_prime(253);
    while(1);
    return;
}
