#ifndef _TIMER0_H_
#define	_TIMER0_H_

#include <pic18f4520.h>
#include <xc.h>


void TIMER0_Initialize(char use_8bit_TF, int prescaler, char interrupt_priority);

void TIMER0_ISR_Enable(char TF);
char TIMER0_ISR(void function(void), char* OUT_state);

#endif	/* _TIMER0_H_ */

