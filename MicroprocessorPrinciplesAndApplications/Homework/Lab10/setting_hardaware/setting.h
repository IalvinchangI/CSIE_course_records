#ifndef SETTING_H
#define	SETTING_H
#include <xc.h>
#include <pic18f4520.h>
//#include "pin_manager.h"
#include "timer2_manage.h"
//#include "tmr1.h"
#include "uart.h"
#include "ccp1.h"
#include "adc.h"
#include "interrupt_manager.h"
#include "button_manager.h"
#include "LED_manager.h"
#include "pin_manager.h"

#define _XTAL_FREQ 4000000

void SYSTEM_Initialize(char, char, char);
void OSCILLATOR_Initialize(void);


#endif