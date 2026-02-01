#include "timer2.h"
#include <stdlib.h>

void TIMER2_Initialize(char postscaler, char prescaler, unsigned char PR2_value, char interrupt_priority) {
    // Postscaler
    if (postscaler > 0 && postscaler <= 16) {
        T2CONbits.T2OUTPS = postscaler - 1;
    } else {
        T2CONbits.T2OUTPS = 0;  // Postscaler 1:1
    }
    // Prescaler
    switch (prescaler) {
        case 4:  // Prescaler 1:4
            T2CONbits.T2CKPS = 1;
            break;
        case 16:  // Prescaler 1:16
            T2CONbits.T2CKPS = 2;
            break;
        case 1:  // Prescaler 1:1
        default:
            T2CONbits.T2CKPS = 0;
            break;
    }
    
    // PR2
    TIMER2_PR2_Set(PR2_value);

    // init value
    TMR2 = 0;
    
    // interrupt
    PIR1bits.TMR2IF = 0;
    IPR1bits.TMR2IP = (interrupt_priority == 1);
    PIE1bits.TMR2IE = 1;

    // start Timer2
    T2CONbits.TMR2ON = 1;
}

void TIMER2_PR2_Set(unsigned char value) {
    PR2 = value;
}
unsigned char TIMER2_PR2_Get(void) {
    return PR2;
}

void TIMER2_ISR_Enable(char TF) {
    PIE1bits.TMR2IE = (TF == 1);
}

char TIMER2_ISR(void function(void), char* OUT_state) {
    if (PIE1bits.TMR2IE && PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        
        if (OUT_state != NULL) {
            *OUT_state = 1;
        }
        if (function != NULL) {  // execute something user define
            function();
        }
    }
    return *OUT_state;
}
