#include <xc.h>
#include <pic18f4520.h>
#include "LED_manager.h"

void LED_RD47_Initialize(void) {
    // setup LED
    LED_RD47_Write(0);
    TRISDbits.RD4 = 0;
    TRISDbits.RD5 = 0;
    TRISDbits.RD6 = 0;
    TRISDbits.RD7 = 0;
}

void LED_RD47_Write(char value) {
    LATD = (LATD & 0x0F) | (value << 4);
}

char LED_RD47_Read(void) {
    return (LATD & 0xF0) >> 4;
}
