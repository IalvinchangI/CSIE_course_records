#ifndef _INTERRUPT_H_
#define	_INTERRUPT_H_

#include <pic18f4520.h>
#include <xc.h>


void INTERRUPT_Initialize(void);

void INTERRUPT_INT0_Initialize(void);
char INTERRUPT_INT0_ISR(void function(void), char* OUT_state);

#endif	/* _INTERRUPT_H_ */

