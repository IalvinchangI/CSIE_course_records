#ifndef _OUTPUTPORTMANAGER_H_
#define	_OUTPUTPORTMANAGER_H_

#include <pic18f4520.h>
#include <xc.h>


#include "portManager.h"

void OUTPUTPORT_RC_Initialize(char mask);
void OUTPUTPORT_RC_Write(char data, char mask);
char OUTPUTPORT_RC_Read(char mask);

void OUTPUTPORT_RD_Initialize(char mask);
void OUTPUTPORT_RD_Write(char data, char mask);
char OUTPUTPORT_RD_Read(char mask);

void OUTPUTPORT_Initialize(char *port, char mask);
void OUTPUTPORT_Write(char *port_latch, char data, char mask);
char OUTPUTPORT_Read(char port_latch, char mask);

#endif	/* _OUTPUTPORTMANAGER_H_ */

