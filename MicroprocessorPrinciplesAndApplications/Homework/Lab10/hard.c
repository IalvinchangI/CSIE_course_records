#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
// using namespace std;


char get_LED_value(int);

char finish_adc = 0;
char finish_timer2 = 0;
unsigned int value = 100;
void main(void)
{
    
    SYSTEM_Initialize(0, 1, 1);
    ADC_Read(0);
    
    while(1) {
        // nothing
    }
    return;
}

char get_LED_value(int adc_value) {
    char mapped_value;
    if (adc_value < 85) {
        mapped_value = 4;
    } 
    else if (adc_value < 170) {
        mapped_value = 5;
    } 
    else if (adc_value < 256) {
        mapped_value = 6;
    } 
    else if (adc_value < 341) {
        mapped_value = 7;
    } 
    else if (adc_value < 426) {
        mapped_value = 8;
    } 
    else if (adc_value < 512) {
        mapped_value = 9;
    } 
    else if (adc_value < 597) {
        mapped_value = 10;
    } 
    else if (adc_value < 682) {
        mapped_value = 11;
    } 
    else if (adc_value < 767) {
        mapped_value = 12;
    } 
    else if (adc_value < 852) {
        mapped_value = 13;
    } 
    else if (adc_value < 938) {
        mapped_value = 14;
    } 
    else {  // [938, 1024)
        mapped_value = 15;
    }
    return mapped_value;
}

void adc_function(int value, int previous_value) {
    if (previous_value != value) {
        // UART
        char str[20] = {};
        strcpy(str, UART_get_TX_buffer());
        for (int i = 0; str[i] != '\0'; i++) {
            while (UART_put_TX_string("\b \b") != 1);
        }
        sprintf(str, "%d", value);
        while (UART_put_TX_string(str) != 1);
        // LED
        char LED_value = get_LED_value(value);
        LED_RD47_Write(LED_value);
    }
    
    // next
    __delay_ms(100);
    ADC_Read(0);
}

void __interrupt(__high_priority) Hi_ISR(void) {
    UART_TX_ISR();
}

void __interrupt(__low_priority)  Lo_ISR(void) {
    ADC_ISR(adc_function, &finish_adc);
}
