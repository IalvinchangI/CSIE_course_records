#include "interrupt.h"
#include <stdlib.h>

void INTERRUPT_Initialize (void) {
    RCONbits.IPEN = 1;      //enable Interrupt Priority mode
    INTCONbits.GIEH = 1;    //enable high priority interrupt
    INTCONbits.GIEL = 1;    //enable low priority interrupt
}

void INTERRUPT_INT0_Initialize(void) {
    // setup INT0 Interrupt
    TRISBbits.TRISB0 = 1;
    INTCON2bits.INTEDG0 = 0;        // INT0 Interrupt on falling edge
    INTCONbits.INT0IF = 0;          // Clean INT0 External Interrupt Flag bit
    INTCONbits.INT0IE = 1;          // Enables the INT0 external interrupt (allow us to use "INT0/RB0 pin" to trigger interrupt)
}

char INTERRUPT_INT0_ISR(void function(void), char* OUT_state) {
    if (INTCONbits.INT0IE && INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;
        
        if (OUT_state != NULL) {
            *OUT_state = 1;
        }
        if (function != NULL) {  // execute something user define
            function();
        }
    }
    return *OUT_state;
}
