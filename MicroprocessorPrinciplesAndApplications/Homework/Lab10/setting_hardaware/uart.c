#include <xc.h>
#include <stdlib.h>
#include "string.h"
#include "uart.h"
    //setting TX/RX


static char rc_buffer[UART_BUFFER_LENGTH];
static char tx_buffer[UART_BUFFER_LENGTH];

void UART_Initialize(char TX_priority, char RC_priority) {
           
    /*       TODObasic   
           Serial Setting      
        1.   Setting Baud rate
        2.   choose sync/async mode 
        3.   enable Serial port (configures RX/DT and TX/CK pins as serial port pins)
        3.5  enable Tx, Rx Interrupt(optional)
        4.   Enable Tx & RX
    */
    // config Tx, Rx as input port
    TRISCbits.TRISC6 = 1;            
    TRISCbits.TRISC7 = 1;            
    
    //  Setting baud rate
    TXSTAbits.SYNC = 0;         // asynchronous mode
    BAUDCONbits.BRG16 = 0;      // use 8bits
    TXSTAbits.BRGH = 0;         // not select high baud rate
    SPBRG = 51;                 // rate = 1202
    
   //   Serial enable
    RCSTAbits.SPEN = 1;         // Serial Port Enable bit
    PIR1bits.TXIF = 0;          // The EUSART transmit buffer is full
    PIR1bits.RCIF = 0;          // The EUSART receive buffer is empty 
    TXSTAbits.TXEN = 1;         // Transmit Enable bit
    RCSTAbits.CREN = 1;         // Continuous Receive Enable bit (if error occurred, it will be clear)
    PIE1bits.TXIE = 0;          // Disable TX interrupts
    PIE1bits.RCIE = 1;          // Enable RC interrupts
    IPR1bits.TXIP = (TX_priority == 1);          // EUSART Transmit Interrupt is Low priority if TX_priority == 0
    IPR1bits.RCIP = (RC_priority == 1);          // EUSART Receive Interrupt is Low priority if RC_priority == 0
}

void UART_Write(unsigned char data) {  // Output on Terminal
    TXREG = data;              //write to TXREG will send data 
}


char UART_Write_Text(char* text) { // Output on Terminal, limit:10 chars
    if (PIE1bits.TXIE != 1) {
        strncpy(tx_buffer, text, UART_BUFFER_LENGTH);
        for(int i = 0; text[i] != '\0'; i++) {
            while(!TXSTAbits.TRMT);
            UART_Write(tx_buffer[i]);
        }
        return 1;  // finish
    }
    return 0;  // error
}

char* UART_get_TX_buffer() {
    return tx_buffer;
}
void UART_clear_TX_buffer() {
    for(int i = 0; i < UART_BUFFER_LENGTH; i++)
        tx_buffer[i] = '\0';
}

char UART_Read() {
    return RCREG;
}

char UART_put_TX_string(char* text) {
    if (PIE1bits.TXIE != 1) {
        strncpy(tx_buffer, text, UART_BUFFER_LENGTH);
        UART_enable_TX_interrupt(1);
        return 1;  // finish
    }
    return 0;  // error
}

void UART_continue_read() {
    UART_enable_RC_interrupt(1);
}
char *UART_get_RC_string(){
    return rc_buffer;
}


/************************ lib ************************/
void UART_enable_RC_interrupt(char TF) {
    PIE1bits.RCIE = (TF == 1);
}
char UART_RC_ISR(char end_input_char, void char_function(char), void finish_function(void), char* out_finish_TF) {
    static unsigned char rc_index = 0;
    if(PIE1bits.RCIE && PIR1bits.RCIF) {
        if(RCSTAbits.OERR) {  // error handling
            CREN = 0;
            Nop();
            CREN = 1;
        }
        
        char output = UART_Read();
        if (char_function != NULL) {
            char_function(output);
        }
        if (output == end_input_char) {
            rc_index = 0;
            *out_finish_TF = 1;
            UART_enable_RC_interrupt(0);
            if (finish_function != NULL) {
                finish_function();
            }
        }
        else {
            if (output == '\b' && rc_index != 0) {  // not work
                rc_index--;
                rc_buffer[rc_index] = '\0';
            }
            else {
                rc_buffer[rc_index] = output;
                rc_index++;
            }
            *out_finish_TF = 0;
        }
    }
    return *out_finish_TF;
}

void UART_enable_TX_interrupt(char TF) {
    PIE1bits.TXIE = (TF == 1);
}
void UART_TX_ISR(void) {
    static unsigned char tx_index = 0;
    if(PIE1bits.TXIE && PIR1bits.TXIF) {
        if (tx_buffer[tx_index] != '\0') {
            UART_Write(tx_buffer[tx_index]);
            tx_index++;
        }
        else {
            tx_index = 0;
            UART_enable_TX_interrupt(0);
        }
    }
    return;
}
