#ifndef _HARDWARETOOLS_H_
#define	_HARDWARETOOLS_H_

#include <pic18f4520.h>
#include <xc.h>

#ifndef _XTAL_FREQ
// default frequency
#define _XTAL_FREQ 4000000
#endif

// frequency is setup by _XTAL_FREQ
void OSCILLATOR_Initialize(unsigned long long);

#include "interrupt.h"
#include "portManager.h"
#include "inputPortManager.h"
#include "outputPortManager.h"
#include "timer0.h"
#include "timer1.h"
#include "timer2.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"

#endif	/* _HARDWARETOOLS_H_ */

