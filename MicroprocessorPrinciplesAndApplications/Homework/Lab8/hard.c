// PIC18F4520 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = INTIO67    // Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <xc.h>
#include <pic18f4520.h>
#define _XTAL_FREQ 125000


/**
 * when F_osc = 125 kHz, T_osc = 8 µs, and prescaler = 4
 */
enum {
    DUTY_CYCLE_NEG_90 = 16, 
    DUTY_CYCLE_POS_90 = 78, 
    DUTY_CYCLE_ZERO = 47, 
    DUTY_CYCLE_INC_STEP = 1, 
    DUTY_CYCLE_DEC_STEP = -1
};
#define setup_duty_cycle(x) \
    CCPR1L = x >> 2;\
    CCP1CONbits.DC1B = x & 3;
#define get_duty_cycle() ((unsigned char)((CCPR1L << 2) + CCP1CONbits.DC1B))


void Buttom_ISR(void);
void Timer0_ISR(void);

volatile char FINISH_INIT = 0;
volatile char button_press = 0;
volatile char active_TF = 0;

void main(void) {
    // INIT
    // Internal Oscillator Frequency, F_osc = 125 kHz, T_osc = 8 µs
    OSCCONbits.IRCF = 0b001;
    
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;
    
    // setup TMR0 Overflow Interrupt
    T0CONbits.TMR0ON = 1;
    T0CONbits.T08BIT = 1;           // 8-bit 
    T0CONbits.T0CS = 0;             // internal clock source
    T0CONbits.PSA = 1;              // not use prescalar
    INTCON2bits.TMR0IP = 0b0;       // Low priority (timer interrupt is high priority)
    INTCONbits.TMR0IF = 0b0;        // Clear TMR0 Overflow Interrupt flag bit (if interrupt occurs, TMR2IF will become 1)
    INTCONbits.TMR0IE = 0b1;        // Enables the TMR0 Overflow interrupt
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /*
     * PWM period
     * = (PR2 + 1) * 4 * T_osc * (TMR2 prescaler)
     * = (0x9B + 1) * 4 * 8µs * 4
     * = 0.019968s ~= 20ms
     * which is equal to how we calculate timer2 interrupt
     * That is, every 4 clockcycle * prescaler, timer2 will add 1
     */
    PR2 = 0x9B;
    
    /*
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * reset = 0
     */
    setup_duty_cycle(DUTY_CYCLE_POS_90);
    
    // setup INT0 Interrupt
    TRISBbits.TRISB0 = 1;
    INTCON2bits.INTEDG0 = 0;        // INT0 Interrupt on falling edge
    INTCONbits.INT0IF = 0;          // Clean INT0 External Interrupt Flag bit
    INTCONbits.INT0IE = 1;          // Enables the INT0 external interrupt (allow us to use "INT0/RB0 pin" to trigger interrupt)
    
    // setup interrupt
    RCONbits.IPEN = 1;          // Enable priority levels on interrupts
    INTCONbits.GIEH = 1;        // Enables all high-priority interrupts (because IPEN=1 and GIEH=1)
    INTCONbits.GIEL = 1;        // Enables all unmasked peripheral interrupts (because IPEN=1 and GIEL=1)
    
    // setup LED
    LATD = 0xFF;
    TRISD = 0;
    
    FINISH_INIT = 0xFF;
    
    
    // waiting
    while (1) {
        if (button_press == 1) {
            Buttom_ISR();
        }
    }
    return;
}

void __interrupt(__high_priority) High_Priority_ISR_Table(void) {
    if (FINISH_INIT == 0xFF) {
        if (INTCONbits.INT0IE && INTCONbits.INT0IF) {
            INTCONbits.INT0IF = 0;
            button_press = 1;
        }
        // else Nothing
    }
    return;
}

void __interrupt(__low_priority) Low_Priority_ISR_Table(void) {
    if (FINISH_INIT == 0xFF) {
        if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
            if (active_TF != 0) {
                Timer0_ISR();
            }
            INTCONbits.TMR0IF = 0b0;
        }
        // else Nothing
    }
    return;
}

void Buttom_ISR(void) {
    active_TF = ~active_TF;
    __delay_ms(300);
    button_press = 0;
    return;
}

void Timer0_ISR(void) {
    static char clockwise_TF = 0;  // clockwise is T -> degree--
    static unsigned char counter = 0;
    if (counter == 4) {  // every 5 interrupt
        LATD = ~LATD;
        unsigned char current = get_duty_cycle();
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
        setup_duty_cycle(current);
        counter = 0;
    }
    else {
        counter++;
    }
    return;
}
