#include <xc.h>

void ADC_Initialize(char priority) {
    TRISA = 0xff;		// Set as input port
    ADCON1 = 0x0e;  	// Ref vtg is VDD & Configure pin as analog pin 
    // ADCON2 = 0x92;  	
    ADCON2bits.ADFM = 1 ;           // Right Justifie
    ADCON0bits.ADON = 1;
    ADCON2bits.ADCS = 7;            // FRC (clock derived from A/D RC oscillator)
    
    // Flush ADC output Register
    ADRESH=0;
    ADRESL=0;
    
    // Setup AD Interrupt
    IPR1bits.ADIP = (priority == 1);
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
}

void ADC_Read(int channel) {
    ADCON0bits.CHS =  channel;  // if 0b0000 => using AN0 as AD input
    ADCON0bits.GO = 1;
}

char ADC_ISR(void function(int, int), char* out_finish_TF) {
    static int previous_value = 0;
    if(PIE1bits.ADIE && PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        int value = (ADRESH << 8) | (ADRESL);
        function(previous_value, value);
        previous_value = value;
    }
    return *out_finish_TF;
}
