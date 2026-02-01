#define _XTAL_FREQ 4000000
#define UART_BUFFER_LENGTH 100

#include "hardwareTools.h"

void main(void) {
    // init
    OSCILLATOR_Initialize(_XTAL_FREQ);
    // Port init
    // INTERRUPT_INT0_Initialize();
    // TIMER0_Initialize(char use_8bit_TF, char prescaler, char interrupt_priority);
    // TIMER1_Initialize(char prescaler, char interrupt_priority);
    // TIMER2_Initialize(char postscaler, char prescaler, unsigned char PR2_value, char interrupt_priority);
    // PWM_Initialize(unsigned char period_value, int duty_cycle_value);
    // ADC_Initialize(char adc_clock_period_value, int acquisition_time_value, char interrupt_priority);
    PORT_Analog_Initialize(PORT_ANALOG_AN00);  // only AN0 is analog
    // UART_Initialize(unsigned char SPBRG_value, char TX_interrupt_priority, char RC_interrupt_priority);
    INTERRUPT_Initialize();
    
    // wait
    while(1) {
        // nothing
    }
    return;
}


void __interrupt(__high_priority) Hi_ISR(void) {
    static char temp;
    return;
}

void __interrupt(__low_priority)  Lo_ISR(void) {
    static char temp;
    return;
}
