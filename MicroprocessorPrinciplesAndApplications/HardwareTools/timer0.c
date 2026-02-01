#include "timer0.h"
#include <stdlib.h>

void TIMER0_Initialize(char use_8bit_TF, int prescaler, char interrupt_priority) {
    // 8bit?
    T0CONbits.T08BIT = (use_8bit_TF == 1);
    
    // Prescaler
    switch (prescaler) {
        case 2:  // Prescaler 1:2
            T0CONbits.T0PS = 0;
            T0CONbits.PSA = 0;  // use prescalar
            break;
        case 4:  // Prescaler 1:4
            T0CONbits.T0PS = 1;
            T0CONbits.PSA = 0;  // use prescalar
            break;
        case 8:  // Prescaler 1:8
            T0CONbits.T0PS = 2;
            T0CONbits.PSA = 0;  // use prescalar
            break;
        case 16:  // Prescaler 1:16
            T0CONbits.T0PS = 3;
            T0CONbits.PSA = 0;  // use prescalar
            break;
        case 32:  // Prescaler 1:32
            T0CONbits.T0PS = 4;
            T0CONbits.PSA = 0;  // use prescalar
            break;
        case 64:  // Prescaler 1:64
            T0CONbits.T0PS = 5;
            T0CONbits.PSA = 0;  // use prescalar
            break;
        case 128:  // Prescaler 1:128
            T0CONbits.T0PS = 6;
            T0CONbits.PSA = 0;  // use prescalar
            break;
        case 256:  // Prescaler 1:256
            T0CONbits.T0PS = 7;
            T0CONbits.PSA = 0;  // use prescalar
            break;
        case 1:  // Prescaler 1:1
        default:
            T0CONbits.PSA = 1;  // not use prescalar
            break;
    }

    // init value
    TMR0L = 0;
    TMR0H = 0;
    
    // interrupt
    INTCONbits.TMR0IF = 0;
    INTCON2bits.TMR0IP = (interrupt_priority == 1);
    INTCONbits.TMR0IE = 1;

    // start Timer0
    T0CONbits.TMR0ON = 1;
}

void TIMER0_ISR_Enable(char TF) {
    INTCONbits.TMR0IE = (TF == 1);
}

char TIMER0_ISR(void function(void), char* OUT_state) {
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
        
        if (OUT_state != NULL) {
            *OUT_state = 1;
        }
        if (function != NULL) {  // execute something user define
            function();
        }
    }
    return *OUT_state;
}
