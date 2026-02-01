#include <xc.h>
#include <pic18f4520.h>
#include <stdio.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = ON    // Brown-out Reset Enable bit
#pragma config PBADEN = OFF  // Watchdog Timer Enable bit
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)


#define _XTAL_FREQ 1000000

#define setup_duty_cycle(x) \
    CCPR1L = x >> 2;\
    CCP1CONbits.DC1B = x & 3;


void __interrupt(high_priority)H_ISR(){
    
    // ADC step4
    int value = ADRESH;
    setup_duty_cycle(value);
    
    
    //clear flag bit
    PIR1bits.ADIF = 0;
    
    return;
}

void main(void) 
{
    // configure OSC and port
    OSCCONbits.IRCF = 0b100;    // 1MHz
    TRISAbits.RA0 = 1;          // analog input port
    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    
    // Timer2 -> On, prescaler -> 16
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b11;
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /*
     * PWM period
     * = (PR2 + 1) * 4 * T_osc * (TMR2 prescaler)
     */
    PR2 = 0xFF;
    /*
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * reset = 0
     */
    setup_duty_cycle(0);
    
    // ADC step1: 
    ADCON1bits.VCFG0 = 0;       // ref+ = VDD
    ADCON1bits.VCFG1 = 0;       // ref- = VSS
    ADCON1bits.PCFG = 0b1110;   // AN0 is analog input. Others are digital
    ADCON0bits.CHS = 0b0000;    // using AN0 as AD input
    ADCON2bits.ADCS = 0b000;    // T_ad = 1Mhz / 2 (period of AD Timer is 2us)
    ADCON2bits.ACQT = 0b001;    // because T_ad = 2us, acquisition time should be set to 2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;        // using AD
    ADCON2bits.ADFM = 0;        // left justified 
    
    
    // ADC step2: Setup AD Interrupt
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    // ADC step3: start to convert
    ADCON0bits.GO = 1;
    
    while(1) {
        if (ADCON0bits.GO == 0) {  // ADC step5 & go back ADC step3
            __delay_us(4);  // delay at least 2tad
            ADCON0bits.GO = 1;
        }
    }
    
    return;
}
