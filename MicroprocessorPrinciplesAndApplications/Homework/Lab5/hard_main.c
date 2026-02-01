/*
 * File:   hard_main.c
 * Author: alvin
 *
 * Created on 2025?10?14?, ?? 3:43
 */


#include <xc.h>

extern long mul_extended(int a, int b);

void main(void){
    volatile long ans = mul_extended(-32768, 32767);
    while(1);
    return;
}
