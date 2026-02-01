LIST p=18f4520
#include<p18f4520.inc>

    CONFIG OSC = INTIO67 ; Set internal oscillator to 1 MHz
    CONFIG WDT = OFF     ; Disable Watchdog Timer
    CONFIG LVP = OFF     ; Disable Low Voltage Programming

    L1 EQU 0x14         ; Define L1 memory location
    L2 EQU 0x15         ; Define L2 memory location
    previous_button_state EQU 0x16
    
    OUTPUT_MASK EQU b'00000111'
    STATE0 EQU b'00000000'
    STATE1 EQU b'00000001'
    STATE2 EQU b'00000010'
    STATE3 EQU b'00000011'
    STATE4 EQU b'00000101'
    
    org 0x00            ; Set program start address to 0x00

; instruction frequency = 1 MHz / 4 = 0.25 MHz
; instruction time = 1/0.25 = 4 us
; Total_cycles = 2 + (2 + 8 * num1 + 3) * num2 cycles
; num1 = 111, num2 = 70, Total_cycles = 62512 cycles
; Total_delay ~= Total_cycles * instruction time = 0.25 s
DELAY macro num1, num2
    local LOOP1         ; Inner loop
    local LOOP2         ; Outer loop
    
    ; 2 cycles
    MOVLW num2          ; Load num2 into WREG
    MOVWF L2            ; Store WREG value into L2
    
    ; Total_cycles for LOOP2 = 2 cycles
    LOOP2:
    MOVLW num1
    MOVWF L1  
    
    ; Total_cycles for LOOP1 = 8 cycles
    LOOP1:
    NOP                 ; busy waiting
    NOP
    NOP
    NOP
    NOP
    DECFSZ L1, 1        
    BRA LOOP1           ; BRA instruction spends 2 cycles
    
    ; 3 cycles
    DECFSZ L2, 1        ; Decrement L2, skip if zero
    BRA LOOP2           
endm
    
Negative_Edge_Trigger macro port_latch, number, temp_register, output
    LOCAL Negative_Edge_Trigger_temp1
    LOCAL Negative_Edge_Trigger_output0
    LOCAL Negative_Edge_Trigger_end
    BTFSC port_latch, number
	BRA Negative_Edge_Trigger_temp1  ; input is 1
    ; input is 0
    BTFSS temp_register, number
	BRA Negative_Edge_Trigger_output0  ; previous is 0
    ; previous is 1
    BSF output, number
    BCF temp_register, number
    BRA Negative_Edge_Trigger_end
    Negative_Edge_Trigger_temp1:
	BSF temp_register, number
    Negative_Edge_Trigger_output0:
	BCF output, number
    Negative_Edge_Trigger_end:
endm

; WARNING: You should check whether the length of bit pattern of new_output is equal to mask
Update_Output_Port macro port_latch, mask, new_output
    MOVLW mask
    COMF WREG
    ANDWF port_latch, F
    MOVF new_output, W
    IORWF port_latch, F
endm
    
movlf macro i, f
    MOVLW i
    MOVWF f
endm

start:
init:
; let pin can receive digital signal
    MOVLW 0x0f          ; Set ADCON1 register for digital mode
    MOVWF ADCON1        ; Store WREG value into ADCON1 register
    CLRF PORTB          ; Clear PORTB
    BSF TRISB, 0        ; Set RB0 as input (TRISB = 0000 0001)
    CLRF LATA           ; Clear LATA
    BCF TRISA, 0        ; Set RA0 as output (TRISA = 0000 0000)
    BCF TRISA, 1        ; Set RA1 as output (TRISA = 0000 0000)
    BCF TRISA, 2        ; Set RA2 as output (TRISA = 0000 0000)
    CLRF previous_button_state
    movlf STATE0, 0x40
    movlf STATE1, 0x41
    movlf STATE2, 0x42
    movlf STATE3, 0x43
    movlf STATE4, 0x44
    
; Button check
check_process:
    DELAY d'12', d'10'        ; around 1 ms
    Negative_Edge_Trigger PORTB, 0, previous_button_state, 0x21
    BTFSS 0x21, 0           ; Check if PORTB bit 0 is trigger (button pressed)
	BRA check_process   ; If button is not pressed, branch back to check_process
    BRA lightup             ; If button is pressed, branch to lightup
    
lightup:
    Update_Output_Port LATA, OUTPUT_MASK, 0x41
    DELAY d'111', d'210' ; Call delay macro to delay for about 0.75 seconds
    Update_Output_Port LATA, OUTPUT_MASK, 0x42
    DELAY d'111', d'210' ; Call delay macro to delay for about 0.75 seconds
    Update_Output_Port LATA, OUTPUT_MASK, 0x43
    DELAY d'111', d'210' ; Call delay macro to delay for about 0.75 seconds
    Update_Output_Port LATA, OUTPUT_MASK, 0x44
    DELAY d'111', d'210' ; Call delay macro to delay for about 0.75 seconds
    Update_Output_Port LATA, OUTPUT_MASK, 0x40
    BRA check_process
end
