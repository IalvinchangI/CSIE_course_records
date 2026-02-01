#include "uart.h"
#include <stdlib.h>
#include "string.h"


static char rc_buffer[UART_BUFFER_LENGTH] = {};
static char tx_buffer[UART_BUFFER_LENGTH] = {};

void UART_Initialize(unsigned char SPBRG_value, char TX_interrupt_priority, char RC_interrupt_priority) {
    // config Tx, Rx as input port
    INPUTPORT_TXRC_Initialize();
    
    //  Setting baud rate
    TXSTAbits.SYNC = 0;         // asynchronous mode
    BAUDCONbits.BRG16 = 0;      // use 8bits
    TXSTAbits.BRGH = 0;         // not select high baud rate
    UART_SPBRG_Set(SPBRG_value);
    
   //   Serial enable
    RCSTAbits.SPEN = 1;         // Serial Port Enable bit
    PIR1bits.TXIF = 0;          // The EUSART transmit buffer is full
    PIR1bits.RCIF = 0;          // The EUSART receive buffer is empty 
    TXSTAbits.TXEN = 1;         // Transmit Enable bit
    RCSTAbits.CREN = 1;         // Continuous Receive Enable bit (if error occurred, it will be clear)
    PIE1bits.TXIE = 0;          // Disable TX interrupts
    PIE1bits.RCIE = 1;          // Enable RC interrupts
    IPR1bits.TXIP = (TX_interrupt_priority == 1);          // EUSART Transmit Interrupt is Low priority if TX_priority == 0
    IPR1bits.RCIP = (RC_interrupt_priority == 1);          // EUSART Receive Interrupt is Low priority if RC_priority == 0
}

/**
 * Formula: SPBRG = [look up the table in DataSheet]
 * This control Baud Rate.
 * p.s. SYNC, BRGH, and BRG16 can also control Baud Rate.
 *      Here, we fixed SYNC = 0, BRGH = 0, BRG16 = 0
 * 
 * @param value is unsigned char
 */
void UART_SPBRG_Set(unsigned char value) {
    SPBRG = value;  // rate = 1202 if SPBRG = 51, F_osc = 4M
}


/************************ write ************************/
// write one byte to TXREG
static void UART_Write(unsigned char byte) {
    TXREG = byte;  // write to TXREG will send data 
}

// send a series of data from TX buffer to UART and enable TX in order to transmit data
char UART_Write_Data(char* data) {
    if (PIE1bits.TXIE != 1) {
        strncpy(tx_buffer, data, UART_BUFFER_LENGTH);
        UART_TX_Interrupt_Enable(1);
        return 1;  // finish
    }
    return 0;  // error
}
char* UART_get_TX_Buffer() {
    return tx_buffer;
}
void UART_clear_TX_buffer() {
    for(int i = 0; i < UART_BUFFER_LENGTH; i++)
        tx_buffer[i] = '\0';
}

char UART_Write_Data_Busy(char* data) {
    if (PIE1bits.TXIE != 1) {
        strncpy(tx_buffer, data, UART_BUFFER_LENGTH);
        for(int i = 0; data[i] != '\0'; i++) {
            while(!TXSTAbits.TRMT);
            UART_Write(tx_buffer[i]);
        }
        return 1;  // finish
    }
    return 0;  // error
}

/************************ read ************************/
// read one byte from RCREG
char UART_Read() {
    return RCREG;
}

// read a series of data from RC buffer
char* UART_get_RC_Buffer() {
    return rc_buffer;
}


/************************ interrupt control ************************/
void UART_continue_read() {
    UART_RC_Interrupt_Enable(1);
}
void UART_RC_Interrupt_Enable(char TF) {
    PIE1bits.RCIE = (TF == 1);
}
char UART_RC_ISR(char end_input_char, void char_function(void), void finish_function(void), char* OUT_state) {
    static unsigned char rc_index = 0;
    if(PIE1bits.RCIE && PIR1bits.RCIF) {
//        PIR1bits.RCIF = 0;
        
        // error handling
        if(RCSTAbits.OERR) {
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        // service
        char output = UART_Read();
        if (output == end_input_char) {
            rc_buffer[rc_index] = '\0';
            rc_index = 0;
            UART_RC_Interrupt_Enable(0);
            
            if (OUT_state != NULL) {
                *OUT_state = 1;
            }
            if (finish_function != NULL) {  // execute something user define
                finish_function();
            }
        }
        else {
            if (output == '\b' && rc_index != 0) {  // not work
                rc_index--;
                rc_buffer[rc_index] = '\0';
            } else {
                rc_buffer[rc_index] = output;
                rc_index++;
            }
            
            if (OUT_state != NULL) {
                *OUT_state = 0;
            }
            if (char_function != NULL) {
                char_function();
            }
        }
    }
    return *OUT_state;
}

void UART_TX_Interrupt_Enable(char TF) {
    PIE1bits.TXIE = (TF == 1);
}
char UART_TX_ISR(void char_function(void), void finish_function(void), char* OUT_state) {
    static unsigned char tx_index = 0;
    if(PIE1bits.TXIE && PIR1bits.TXIF) {
//        PIR1bits.TXIF = 0;
        
        // service
        if (tx_buffer[tx_index] != '\0') {
            UART_Write(tx_buffer[tx_index]);
            tx_index++;
            
            if (OUT_state != NULL) {
                *OUT_state = 0;
            }
            if (char_function != NULL) {
                char_function();
            }
        }
        else {
            tx_index = 0;
            UART_TX_Interrupt_Enable(0);
            
            if (OUT_state != NULL) {
                *OUT_state = 1;
            }
            if (finish_function != NULL) {  // execute something user define
                finish_function();
            }
        }
    }
    return *OUT_state;
}
