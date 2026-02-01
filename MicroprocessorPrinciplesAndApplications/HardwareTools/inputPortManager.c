#include "inputPortManager.h"

void INPUTPORT_Initialize(char *port, char mask) {
    PORT_Initialize(port, mask, PORT_INPUT_SETUP);
}

char INPUTPORT_Read(char port_latch, char mask) {
    return PORT_Read(port_latch, mask);
}


void INPUTPORT_TXRC_Initialize(void) {
    PORT_Initialize(&TRISC, PORT_MASK_6 | PORT_MASK_7, PORT_INPUT_SETUP);
}
