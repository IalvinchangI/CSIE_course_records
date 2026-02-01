#include <xc.h>
#include <pic18f4520.h>
#include "button_manager.h"

void BUTTON_INT0_Initialize (void) {
    // setup INT0 Interrupt
    TRISBbits.TRISB0 = 1;
    INTCON2bits.INTEDG0 = 0;        // INT0 Interrupt on falling edge
    INTCONbits.INT0IF = 0;          // Clean INT0 External Interrupt Flag bit
    INTCONbits.INT0IE = 1;          // Enables the INT0 external interrupt (allow us to use "INT0/RB0 pin" to trigger interrupt)
}

char BUTTON_INT0_ISR(char *button_press) {
    if (INTCONbits.INT0IE && INTCONbits.INT0IF) {
        *button_press = 1;
        INTCONbits.INT0IF = 0;
    }
    return *button_press;
}


