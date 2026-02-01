#include "pwm.h"
#include <stdlib.h>

void PWM_Initialize(unsigned char period_value, int duty_cycle_value) {
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    // CCP1/RC2 -> Output
    OUTPUTPORT_RC_Initialize(PORT_MASK_2);
    
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    PWM_Period_Set(period_value);
    PWM_Duty_Cycle_Set(duty_cycle_value);
}

/**
 * Formula: PWM period = (value + 1) * 4 * T_osc * (TMR2 prescaler)
 * p.s. T_osc is period of oscillator
 * 
 * @param value only 8 bits
 */
void PWM_Period_Set(unsigned char value) {
    TIMER2_PR2_Set(value);
}
unsigned char PWM_Period_Get(void) {
    return TIMER2_PR2_Get();
}

/**
 * Formula: Duty cycle = value * T_osc * (TMR2 prescaler)
 * p.s. T_osc is period of oscillator
 * 
 * @param value only 10 bits
 */
void PWM_Duty_Cycle_Set(int value) {
    CCPR1L = value >> 2;
    CCP1CONbits.DC1B = value & 3;
}
int PWM_Duty_Cycle_Get(void) {
    return ((int)CCPR1L << 2) + CCP1CONbits.DC1B;
}
