#ifndef _PWM_H
#define	_PWM_H

#include <pic18f4520.h>
#include <xc.h>
#include "outputPortManager.h"
#include "timer2.h"

// use CCP1/RC2
void PWM_Initialize(unsigned char period_value, int duty_cycle_value);

/**
 * Formula: PWM period = (value + 1) * 4 * T_osc * (TMR2 prescaler)
 * p.s. T_osc is period of oscillator
 * 
 * @param value only 8 bits
 */
void PWM_Period_Set(unsigned char value);
unsigned char PWM_Period_Get(void);

/**
 * Formula: Duty cycle = value * T_osc * (TMR2 prescaler)
 * p.s. T_osc is period of oscillator
 * 
 * @param value only 10 bits
 */
void PWM_Duty_Cycle_Set(int value);
int PWM_Duty_Cycle_Get(void);

#endif	/* _PWM_H_ */

