#ifndef _PORTMANAGER_H_
#define	_PORTMANAGER_H_

#include <pic18f4520.h>
#include <xc.h>


#define PORT_MASK_47 (0xF0)
#define PORT_MASK_03 (0x0F)
#define PORT_MASK_0  (0b00000001)
#define PORT_MASK_1  (0b00000010)
#define PORT_MASK_2  (0b00000100)
#define PORT_MASK_3  (0b00001000)
#define PORT_MASK_4  (0b00010000)
#define PORT_MASK_5  (0b00100000)
#define PORT_MASK_6  (0b01000000)
#define PORT_MASK_7  (0b10000000)

#define PORT_INPUT_SETUP  (0xFF)
#define PORT_OUTPUT_SETUP (0x00)


void PORT_Initialize(char *port, char mask, char input_TF);
void PORT_Write(char *port_latch, char data, char mask);
char PORT_Read(char port_latch, char mask);


///////////////////////////////////////////////////////////////////////////////

#define PORT_DIGITAL     (0x0F)
#define PORT_ANALOG_AN00 (0x0E)
#define PORT_ANALOG_AN01 (0x0D)
#define PORT_ANALOG_AN02 (0x0C)
#define PORT_ANALOG_AN03 (0x0B)
#define PORT_ANALOG_AN04 (0x0A)
#define PORT_ANALOG_AN05 (0x09)
#define PORT_ANALOG_AN06 (0x08)
#define PORT_ANALOG_AN07 (0x07)
#define PORT_ANALOG_AN08 (0x06)
#define PORT_ANALOG_AN09 (0x05)
#define PORT_ANALOG_AN0A (0x04)
#define PORT_ANALOG_AN0B (0x03)
#define PORT_ANALOG_AN0C (0x02)


void PORT_Analog_Initialize(char analog_port);

#endif	/* _PORTMANAGER_H_ */

