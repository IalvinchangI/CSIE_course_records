#include "outputPortManager.h"

void OUTPUTPORT_RC_Initialize(char mask) {
    OUTPUTPORT_Initialize(&TRISC, mask);
    OUTPUTPORT_RC_Write(0, mask);
}
void OUTPUTPORT_RC_Write(char data, char mask) {
    OUTPUTPORT_Write(&LATC, data, mask);
}
char OUTPUTPORT_RC_Read(char mask) {
    return OUTPUTPORT_Read(LATC, mask);
}


void OUTPUTPORT_RD_Initialize(char mask) {
    OUTPUTPORT_Initialize(&TRISD, mask);
    OUTPUTPORT_RD_Write(0, mask);
}
void OUTPUTPORT_RD_Write(char data, char mask) {
    OUTPUTPORT_Write(&LATD, data, mask);
}
char OUTPUTPORT_RD_Read(char mask) {
    return OUTPUTPORT_Read(LATD, mask);
}


void OUTPUTPORT_Initialize(char *port, char mask) {
    PORT_Initialize(port, mask, PORT_OUTPUT_SETUP);
}
void OUTPUTPORT_Write(char *port_latch, char data, char mask) {
    PORT_Write(port_latch, data, mask);
}
char OUTPUTPORT_Read(char port_latch, char mask) {
    return PORT_Read(port_latch, mask);
}
