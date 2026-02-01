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

void __interrupt(high_priority)H_ISR(){
    
    //step4
    int value = ADRESH;
    
    
    //do things
    
    
    //clear flag bit
    PIR1bits.ADIF = 0;
    
    
    //step5 & go back step3
    /*
    delay at least 2tad
    ADCON0bits.GO = 1;
    */
    
    return;
}

void main(void) 
{
    // configure OSC and port
    OSCCONbits.IRCF = 0b100;    // 1MHz
    TRISAbits.RA0 = 1;          // analog input port
    // LED output port
    TRISCbits.RC0 = 0;
    TRISCbits.RC1 = 0;
    TRISCbits.RC2 = 0;
    TRISCbits.RC3 = 0;
    LATC = 0;
    
    // step1: 
    ADCON1bits.VCFG0 = 0;       // ref+ = VDD
    ADCON1bits.VCFG1 = 0;       // ref- = VSS
    ADCON1bits.PCFG = 0b1110;   // AN0 is analog input. Others are digital
    ADCON0bits.CHS = 0b0000;    // using AN0 as AD input
    ADCON2bits.ADCS = 0b000;    // T_ad = 1Mhz / 2 (period of AD Timer is 2us)
    ADCON2bits.ACQT = 0b001;    // because T_ad = 2us, acquisition time should be set to 2Tad = 4 > 2.4
    ADCON0bits.ADON = 1;        // using AD
    ADCON2bits.ADFM = 0;        // left justified 
    
    
    // step2: Setup AD Interrupt
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    // step3: start to convert
    ADCON0bits.GO = 1;
    
    while(1);
    
    return;
}
