#ifndef _INPUTPORTMANAGER_H_
#define	_INPUTPORTMANAGER_H_

#include <pic18f4520.h>
#include <xc.h>


#include "portManager.h"

void INPUTPORT_Initialize(char *port, char mask);
char INPUTPORT_Read(char port_latch, char mask);

void INPUTPORT_TXRC_Initialize(void);

#endif	/* _INPUTPORTMANAGER_H_ */

