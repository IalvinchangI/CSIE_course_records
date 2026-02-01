#include "portManager.h"


void PORT_Initialize(char *port, char mask, char input_TF) {
    char temp_port = *port & ~mask;  // clear position
    *port = temp_port | (mask & input_TF);  // setup
}


static void __Shift_Write_Data(char *data, char mask) {
    for (char i = 1; ((mask & i) == 0) && (i != 0); i = i << 1) {
        *data = *data << 1;
    }
}
void PORT_Write(char *port_latch, char data, char mask){
    char temp_latch = *port_latch & ~mask;  // clear position
//    __Shift_Write_Data(&data, mask);  // adjust data
    *port_latch = temp_latch | (mask & data);  // write
    
}

static void __Shift_Read_Data(char *data, char mask) {
    if (*data != 0 && mask != 0) {
        for (char i = 1; ((mask & i) == 0); mask = mask >> 1) {
            *data = *data >> 1;
        }
    }
}
char PORT_Read(char port_latch, char mask) {
    char data = port_latch & mask;  // read
//    __Shift_Read_Data(&data, mask);  // adjust data
    return data;
}


void PORT_Analog_Initialize(char analog_port) {
    ADCON1bits.PCFG = analog_port;
}
