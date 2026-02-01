#include "timer1.h"
#include <stdlib.h>

void TIMER1_Initialize(char prescaler, char interrupt_priority) {
    // Prescaler
    switch (prescaler) {
        case 2:  // Prescaler 1:2
            T1CONbits.T1CKPS = 1;
            break;
        case 4:  // Prescaler 1:4
            T1CONbits.T1CKPS = 2;
            break;
        case 8:  // Prescaler 1:8
            T1CONbits.T1CKPS = 3;
            break;
        case 1:  // Prescaler 1:1
        default:
            T1CONbits.T1CKPS = 0;
            break;
    }

    // init value
    TMR1L = 0;
    TMR1H = 0;
    
    // interrupt
    PIR1bits.TMR1IF = 0;
    IPR1bits.TMR1IP = (interrupt_priority == 1);
    PIE1bits.TMR1IE = 1;

    // start Timer1
    T1CONbits.TMR1ON = 1;
}

void TIMER1_ISR_Enable(char TF) {
    PIE1bits.TMR1IE = (TF == 1);
}

char TIMER1_ISR(void function(void), char* OUT_state) {
    if (PIE1bits.TMR1IE && PIR1bits.TMR1IF) {
        PIR1bits.TMR1IF = 0;
        
        if (OUT_state != NULL) {
            *OUT_state = 1;
        }
        if (function != NULL) {  // execute something user define
            function();
        }
    }
    return *OUT_state;
}
