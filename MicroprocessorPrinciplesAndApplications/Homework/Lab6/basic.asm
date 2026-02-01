LIST p=18f4520
#include<p18f4520.inc>

    CONFIG OSC = INTIO67 ; Set internal oscillator to 1 MHz
    CONFIG WDT = OFF     ; Disable Watchdog Timer
    CONFIG LVP = OFF     ; Disable Low Voltage Programming

    L1 EQU 0x14         ; Define L1 memory location
    L2 EQU 0x15         ; Define L2 memory location
    previous_button_state EQU 0x16
    
    OUTPUT_MASK EQU b'00000111'
    OUTPUT_CLR  EQU b'11111000'
    
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
    
; Button check
check_process:
    DELAY d'12', d'10'        ; around 1 ms
    Negative_Edge_Trigger PORTB, 0, previous_button_state, 0x21
    BTFSS 0x21, 0           ; Check if PORTB bit 0 is trigger (button pressed)
	BRA check_process   ; If button is not pressed, branch back to check_process
    BRA lightup             ; If button is pressed, branch to lightup
    
lightup:
    DELAY d'111', d'70' ; Call delay macro to delay for about 0.25 seconds
    MOVLW OUTPUT_MASK
    ANDWF LATA, W
    BZ reset_output
    RLNCF WREG, W
    MOVWF 0x20
    MOVLW OUTPUT_CLR
    ANDWF LATA, F
    MOVLW OUTPUT_MASK
    ANDWF 0x20, W
    IORWF LATA, F
    BRA check_process
    reset_output:
    BSF LATA, 0
    BRA check_process
end
