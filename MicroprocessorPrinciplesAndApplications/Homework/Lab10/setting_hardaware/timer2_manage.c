#include <xc.h>
#include <stdlib.h>
#include "timer2_manage.h"

void TIMER2_Initialize(char priority) {
    // scalar
    T2CONbits.T2OUTPS = 4;  // Postscaler 1:5
    T2CONbits.T2CKPS = 2;   // Prescaler 1:16

    // 10ms: (124+1) * 16 * 5 * (1/4us * 4) = 10000us = 10ms
    PR2 = 124;

    // init value
    TMR2 = 0;
    
    // interrupt
    PIR1bits.TMR2IF = 0;
    IPR1bits.TMR2IP = (priority == 1);
    PIE1bits.TMR2IE = 1;

    // start Timer2
    T2CONbits.TMR2ON = 1;
}

char TIMER2_ISR(unsigned int times, void function(void), char *out_finish_TF) {
    static int tick_count = 0;
    if (PIE1bits.TMR2IE && PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        tick_count++;
        
        if (tick_count >= times) {  // finish?
            if (function != NULL) {  // execute something user define
                function();
            }
            tick_count = 0;
            *out_finish_TF = 1;
        }
        else {
            *out_finish_TF = 0;
        }
    }
    return *out_finish_TF;
}
