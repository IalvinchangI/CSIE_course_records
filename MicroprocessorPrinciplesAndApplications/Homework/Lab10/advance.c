#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
// using namespace std;

char *str = NULL;

char finish_rc = 0;
char finish_timer2 = 0;
unsigned int value = 100;
void main(void) 
{
    
    SYSTEM_Initialize(0, 1, 1);
    TIMER2_Initialize(0);
    
    while(1) {
        // nothing
    }
    return;
}

void timer2_function(void) {
    LED_RD47_Write(LED_RD47_Read() + 1);
}

void uart_rc_char_function(char word) {
    char str[3] = {word};
    if (word == '\r') {
        str[1] = '\n';
    }
    UART_put_TX_string(str);
}

void uart_rc_finish_function(void) {
    str = UART_get_RC_string();
    double temp = atof(str);
    value = temp * 100;
    UART_continue_read();
}

void __interrupt(__high_priority) Hi_ISR(void) {
    UART_TX_ISR();
    UART_RC_ISR('\r', uart_rc_char_function, uart_rc_finish_function, &finish_rc);
}

void __interrupt(__low_priority)  Lo_ISR(void) {
    TIMER2_ISR(value, timer2_function, &finish_timer2);
}
