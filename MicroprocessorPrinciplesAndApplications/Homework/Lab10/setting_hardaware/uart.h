#ifndef _UART_H
#define _UART_H

#define UART_BUFFER_LENGTH 20
      
void UART_Initialize(char, char);

void UART_Write(unsigned char);

char UART_Write_Text(char*);
char* UART_get_TX_buffer();
void UART_clear_TX_buffer();

char UART_Read();

char UART_put_TX_string(char*);

void UART_continue_read();
char *UART_get_RC_string();


/************************ lib ************************/
void UART_enable_RC_interrupt(char);
char UART_RC_ISR(char, void (char), void (void), char*);

void UART_enable_TX_interrupt(char);
void UART_TX_ISR(void);

#endif