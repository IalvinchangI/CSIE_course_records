#ifndef _TIMER2_H_
#define	_TIMER2_H_

#include <pic18f4520.h>
#include <xc.h>


void TIMER2_Initialize(char postscaler, char prescaler, unsigned char PR2_value, char interrupt_priority);

void TIMER2_PR2_Set(unsigned char value);
unsigned char TIMER2_PR2_Get(void);

void TIMER2_ISR_Enable(char TF);
char TIMER2_ISR(void function(void), char* OUT_state);

#endif	/* _TIMER2_H_ */

