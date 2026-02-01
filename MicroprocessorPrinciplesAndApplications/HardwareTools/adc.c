#include "adc.h"
#include <stdlib.h>

void ADC_Initialize(char adc_clock_period_value, int acquisition_time_value, char interrupt_priority) {
    // Setup ADC
    ADCON1bits.VCFG0 = 0;       // ref+ = VDD
    ADCON1bits.VCFG1 = 0;       // ref- = VSS
    ADCON2bits.ADFM = 1 ;       // Right Justifie
    ADC_Clock_Set(adc_clock_period_value);
    ADC_Acquisition_Time_Set(acquisition_time_value);
    
    // Flush ADC output Register
    ADRESH=0;
    ADRESL=0;
    
    // Setup AD Interrupt
    IPR1bits.ADIP = (interrupt_priority == 1);
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    
    // use ADC
    ADCON0bits.ADON = 1;
}

/**
 * Formula: T_ad = ADC clock period = value * T_osc
 * p.s. T_osc is period of oscillator
 * p.s. T_ad > 0.7us
 * 
 * @param value is between 1 and 64, and it should be the power of 2
 * p.s. if value == 1 => use FRC (period = 1.2us ~ 2.5us)
 */
void ADC_Clock_Set(char value) {
    switch (value) {
        case 2:
            ADCON2bits.ADCS = 0;
            break;
        case 4:
            ADCON2bits.ADCS = 4;
            break;
        case 8:
            ADCON2bits.ADCS = 1;
            break;
        case 16:
            ADCON2bits.ADCS = 5;
            break;
        case 32:
            ADCON2bits.ADCS = 2;
            break;
        case 64:
            ADCON2bits.ADCS = 6;
            break;
        case 1:
        default:
            ADCON2bits.ADCS = 7;
            break;
    }
}

/**
 * Formula: Acquisition Time = value * T_ad
 * p.s. T_osc is period of oscillator
 * p.s. Acquisition Time > 2.4us
 * 
 * @param value should be 0, 2, 4, 6, 8, 12, 16, 20
 */
void ADC_Acquisition_Time_Set(unsigned char value) {
    switch (value) {
        case 2:
            ADCON2bits.ACQT = 1;
            break;
        case 4:
            ADCON2bits.ACQT = 2;
            break;
        case 6:
            ADCON2bits.ACQT = 3;
            break;
        case 8:
            ADCON2bits.ACQT = 4;
            break;
        case 12:
            ADCON2bits.ACQT = 5;
            break;
        case 16:
            ADCON2bits.ACQT = 6;
            break;
        case 20:
            ADCON2bits.ACQT = 7;
            break;
        case 0:
        default:
            ADCON2bits.ACQT = 0;
            break;
    }
}


void ADC_Start_Read(int channel) {
    ADCON0bits.CHS = channel;    // if channel == 0 using AN0 as AD input
    ADCON0bits.GO = 1;
}

int ADC_Value_Get(void) {
    return (ADRESH << 8) | (ADRESL);
}

char ADC_ISR(void function(void), char* OUT_state) {
    if(PIE1bits.ADIE && PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        
        if (OUT_state != NULL) {
            *OUT_state = 1;
        }
        if (function != NULL) {  // execute something user define
            function();
        }
    }
    return *OUT_state;
}
