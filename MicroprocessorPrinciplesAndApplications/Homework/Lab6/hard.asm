LIST p=18f4520
#include<p18f4520.inc>

    CONFIG OSC = INTIO67 ; Set internal oscillator to 1 MHz
    CONFIG WDT = OFF     ; Disable Watchdog Timer
    CONFIG LVP = OFF     ; Disable Low Voltage Programming

    L1 EQU 0x14         ; Define L1 memory location
    L2 EQU 0x15         ; Define L2 memory location
    previous_button_state EQU 0x16
    button_trigger EQU 0x17
    counter EQU 0x20
    state_length EQU 0x21
    t1 EQU 0x61
    
    OUTPUT_MASK EQU b'00000111'
    STATE0   EQU b'00000000'
    STATE1_1 EQU b'00000001'
    STATE1_2 EQU b'00000010'
    STATE1_3 EQU b'00000100'
    STATE2_1 EQU b'00000001'
    STATE2_2 EQU b'00000001'
    STATE2_3 EQU b'00000011'
    STATE2_4 EQU b'00000011'
    STATE2_5 EQU b'00000100'
    STATE2_6 EQU b'00000000'
    STATE2_7 EQU b'00000000'
    STATE2_8 EQU b'00000100'
    STATE2_9 EQU b'00000000'
    STATE2_A EQU b'00000000'
    STATE2_B EQU b'00000100'
    STATE0_LENGTH EQU 0x01
    STATE1_LENGTH EQU 0x03
    STATE2_LENGTH EQU 0x0B
    STATE0_START EQU 0x30
    STATE1_START EQU 0x31
    STATE2_START EQU 0x34

    org 0x00            ; Set program start address to 0x00

; instruction frequency = 1 MHz / 4 = 0.25 MHz
; instruction time = 1/0.25 = 4 us
; Total_cycles = 2 + (2 + 3 + 10 * num1 + 2 + 3) * num2 cycles
DELAY macro num1, num2, port_latch, number, temp_register, trigger_and_goto
    local LOOP1         ; Inner loop
    local LOOP2         ; Outer loop
    
    ; 2 cycles
    MOVLW num2          ; Load num2 into WREG
    MOVWF L2            ; Store WREG value into L2
    
    ; Total_cycles for LOOP2 = 2 cycles
    LOOP2:
    MOVLW num1
    MOVWF L1
    
    NOP
    NOP
    NOP
    
    ; Total_cycles for LOOP1 = 10 cycles
    LOOP1:
    Negative_Edge_Trigger port_latch, number, temp_register, trigger_and_goto
    DECFSZ L1, 1        
    BRA LOOP1           ; BRA instruction spends 2 cycles
    
    ; BRA +1 cycle
    NOP
    
    ; 3 cycles
    DECFSZ L2, 1        ; Decrement L2, skip if zero
    BRA LOOP2           
endm
    
Negative_Edge_Trigger macro port_latch, number, temp_register, trigger_and_goto
    ; spend 8
    LOCAL Negative_Edge_Trigger_temp1
    LOCAL Negative_Edge_Trigger_end
    NOP						    ; 1 cycle
    BTFSC port_latch, number				    ; 2 cycle
	BRA Negative_Edge_Trigger_temp1  ; input is 1	    ; (+1 cycle)
    ; input is 0
    BTFSS temp_register, number			    ; 2 cycle
	BRA Negative_Edge_Trigger_end  ; previous is 0	    ; (+1 cycle)
    ; previous is 1
    BCF temp_register, number				    ; 1 cycle
    GOTO trigger_and_goto				    ; 2 cycle
    Negative_Edge_Trigger_temp1:			    ; 2 cycle
	BSF temp_register, number			    ; (1 cycle)
	NOP						    ; (1 cycle)
    Negative_Edge_Trigger_end:				    ; 2 cycle
    NOP						    ; (1 cycle)
    NOP						    ; (1 cycle)
endm

; WARNING: You should check whether the length of bit pattern of new_output is equal to mask
Update_Output_Port macro port_latch, mask, new_output
    MOVFF new_output, t1
    MOVLW mask
    COMF WREG
    ANDWF port_latch, W
    IORWF t1, F
    MOVFF t1, port_latch
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
    movlf STATE0  , 0x30  ; STATE0_START
    movlf STATE1_1, 0x31  ; STATE1_START
    movlf STATE1_2, 0x32
    movlf STATE1_3, 0x33
    movlf STATE2_1, 0x34  ; STATE2_START
    movlf STATE2_2, 0x35
    movlf STATE2_3, 0x36
    movlf STATE2_4, 0x37
    movlf STATE2_5, 0x38
    movlf STATE2_6, 0x39
    movlf STATE2_7, 0x3A
    movlf STATE2_8, 0x3B
    movlf STATE2_9, 0x3C
    movlf STATE2_A, 0x3D
    movlf STATE2_B, 0x3E
    CLRF counter
    movlf STATE0_LENGTH, state_length
    LFSR 0, STATE0_START
    
; Button check
check_process_and_lightup:
    MOVF counter, W
    Update_Output_Port LATA, OUTPUT_MASK, PLUSW0
    INCF counter, W
    CPFSEQ state_length
	BRA continue  ; not equal
    ; equal
    CLRF WREG
    continue:
    MOVWF counter
    ; 1. Call delay macro to delay for about 0.5 seconds
    ; 2. Check if PORTB bit 0 is trigger (button pressed)
    DELAY d'99', d'125', PORTB, 0, previous_button_state, update_state
    BRA check_process_and_lightup   ; If button is not pressed, branch back to check_process                  
    
update_state:  ; If button is pressed, branch to update_state
    CLRF counter
    MOVLW STATE0_LENGTH
    CPFSEQ state_length
	BRA update_state_to_1_2  ; not equal
    BRA update_state_to_1  ; equal to 0
    update_state_to_1_2:
    MOVLW STATE1_LENGTH
    CPFSEQ state_length
	BRA update_state_to_0  ; not equal (equal to 2)
    BRA update_state_to_2  ; equal to 1
    
    update_state_to_0:
        LFSR 0, STATE0_START
	movlf STATE0_LENGTH, state_length
	BRA check_process_and_lightup
    update_state_to_1:
        LFSR 0, STATE1_START
	movlf STATE1_LENGTH, state_length
	BRA check_process_and_lightup
    update_state_to_2:
        LFSR 0, STATE2_START
	movlf STATE2_LENGTH, state_length
	BRA check_process_and_lightup
    
end
