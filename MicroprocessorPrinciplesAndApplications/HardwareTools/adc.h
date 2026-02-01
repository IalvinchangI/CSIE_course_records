#ifndef _ADC_H_
#define	_ADC_H_

#include <pic18f4520.h>
#include <xc.h>


void ADC_Initialize(char adc_clock_period_value, int acquisition_time_value, char interrupt_priority);

/**
 * Formula: T_ad = ADC clock period = value * T_osc
 * p.s. T_osc is period of oscillator
 * p.s. T_ad > 0.7us
 * 
 * @param value is between 1 and 64, and it should be the power of 2
 */
void ADC_Clock_Set(char value);

/**
 * Formula: Acquisition Time = value * T_ad
 * p.s. T_osc is period of oscillator
 * p.s. Acquisition Time > 2.4us
 * 
 * @param value should be 0, 2, 4, 6, 8, 12, 16, 20
 */
void ADC_Acquisition_Time_Set(unsigned char value);


void ADC_Start_Read(int channel);

int ADC_Value_Get(void);

char ADC_ISR(void function(void), char* OUT_state);

#endif	/* _ADC_H_ */

