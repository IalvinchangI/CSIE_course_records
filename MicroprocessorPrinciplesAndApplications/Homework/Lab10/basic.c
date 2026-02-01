#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
// using namespace std;

char str[20] = {};


char button_press = 0;
unsigned int value = 0;
void main(void) 
{
    
    SYSTEM_Initialize(0, 0, 0);
    strcpy(str, "0");
    while (UART_put_TX_string(str) != 1);
    
    while(1) {
        if (button_press == 1) {
            value++;
            // LED
            LED_RD47_Write((char)value);
            
            // UART
            for (int i = 0; str[i] != '\0'; i++) {
                while (UART_put_TX_string("\b") != 1);
            }
            sprintf(str, "%u", value);
            while (UART_put_TX_string(str) != 1);
            __delay_ms(300);
            button_press = 0;
        }
    }
    return;
}

void __interrupt(__high_priority) Hi_ISR(void)
{
    BUTTON_INT0_ISR(&button_press);
}

void __interrupt(low_priority)  Lo_ISR(void)
{
    UART_TX_ISR();
    return;
}
