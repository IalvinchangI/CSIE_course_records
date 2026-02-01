#ifndef TIMER2_MANAGE_H
#define	TIMER2_MANAGE_H

void TIMER2_Initialize(char);

char TIMER2_ISR(unsigned int, void (void), char*);

#endif	/* TIMER2_MANAGE_H */

