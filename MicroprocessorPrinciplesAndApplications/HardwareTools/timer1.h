#ifndef _TIMER1_H_
#define	_TIMER1_H_

#include <pic18f4520.h>
#include <xc.h>


void TIMER1_Initialize(char prescaler, char interrupt_priority);

void TIMER1_ISR_Enable(char TF);
char TIMER1_ISR(void function(void), char* OUT_state);

#endif	/* _TIMER1_H_ */

