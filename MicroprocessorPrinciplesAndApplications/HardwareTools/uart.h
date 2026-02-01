#ifndef _UART_H_
#define	_UART_H_

#include <pic18f4520.h>
#include <xc.h>
#include "inputPortManager.h"


#ifndef UART_BUFFER_LENGTH
// default UART buffer length
#define UART_BUFFER_LENGTH 100
#endif


void UART_Initialize(unsigned char SPBRG_value, char TX_interrupt_priority, char RC_interrupt_priority);

/**
 * Formula: SPBRG = [look up the table in DataSheet]
 * This control Baud Rate.
 * p.s. SYNC, BRGH, and BRG16 can also control Baud Rate.
 *      Here, we fixed SYNC = 0, BRGH = 0, BRG16 = 0
 * 
 * @param value is unsigned char
 */
void UART_SPBRG_Set(unsigned char value);


/************************ write ************************/
// send a series of data from TX buffer to UART and enable TX in order to transmit data
char UART_Write_Data(char* data);
char* UART_get_TX_Buffer();
void UART_clear_TX_buffer();

char UART_Write_Data_Busy(char* data);

/************************ read ************************/
// read one byte from RCREG
char UART_Read();

// read a series of data from RC buffer
char* UART_get_RC_Buffer();


/************************ interrupt control ************************/
void UART_continue_read();
void UART_RC_Interrupt_Enable(char TF);
char UART_RC_ISR(char end_input_char, void char_function(void), void finish_function(void), char* OUT_state);

void UART_TX_Interrupt_Enable(char TF);
char UART_TX_ISR(void char_function(void), void finish_function(void), char* OUT_state);

#endif	/* _UART_H_ */

