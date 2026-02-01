#define _XTAL_FREQ 500000
#define UART_BUFFER_LENGTH 100

#include "hardwareTools.h"

void Button_function(void);
void Timer2_function(void);


/**
 * when F_osc = 0.5 MHz, T_osc = 2 µs, and prescaler = 16
 */
enum {
    DUTY_CYCLE_NEG_90 = 16, 
    DUTY_CYCLE_POS_90 = 78, 
    DUTY_CYCLE_ZERO = 47, 
    DUTY_CYCLE_INC_STEP = 1, 
    DUTY_CYCLE_DEC_STEP = -1
};


volatile char active_TF = 0;

void main(void) {
    // init
    OSCILLATOR_Initialize(_XTAL_FREQ);
    // Port init
    OUTPUTPORT_RD_Initialize(PORT_MASK_47);
    OUTPUTPORT_RD_Write(0, PORT_MASK_47);
    INTERRUPT_INT0_Initialize();
    // TIMER0_Initialize(char use_8bit_TF, char prescaler, char interrupt_priority);
    // TIMER1_Initialize(char prescaler, char interrupt_priority);
    TIMER2_Initialize(0, 16, 0x9B, 0);
    PWM_Initialize(0x9B, DUTY_CYCLE_ZERO);
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
    INTERRUPT_INT0_ISR(Button_function, &temp);
    return;
}

void __interrupt(__low_priority)  Lo_ISR(void) {
    static char temp;
    TIMER2_ISR(Timer2_function, &temp);
    return;
}



void Button_function(void) {
    active_TF = ~active_TF;
    OUTPUTPORT_RD_Write(~OUTPUTPORT_RD_Read(PORT_MASK_47), PORT_MASK_47);
    __delay_ms(300);
    return;
}

void Timer2_function(void) {
    static char clockwise_TF = 0;  // clockwise is T -> degree--
    static unsigned char counter = 0;
    if (active_TF == 0xFF) {
        if (counter == 4) {  // every 5 interrupt
            int current = PWM_Duty_Cycle_Get();
            if (clockwise_TF == 1) {
                if (current + DUTY_CYCLE_INC_STEP >= DUTY_CYCLE_POS_90) {
                    current = DUTY_CYCLE_POS_90;
                    clockwise_TF = 0;
                }
                else {
                    current += DUTY_CYCLE_INC_STEP;
                }
            }
            else {
                if (current - DUTY_CYCLE_INC_STEP <= DUTY_CYCLE_NEG_90) {
                    current = DUTY_CYCLE_NEG_90;
                    clockwise_TF = 1;
                }
                else {
                    current -= DUTY_CYCLE_INC_STEP;
                }
            }
            PWM_Duty_Cycle_Set(current);
            counter = 0;
        }
        else {
            counter++;
        }
    }
    return;
}
