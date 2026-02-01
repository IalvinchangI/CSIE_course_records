#INCLUDE <p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00    ;PC = 0x00 

    ; # setup
    ; data A
    MOVLW 0xFE  ; <-- edit
    MOVWF 0x00
    ; data B
    MOVLW 0xFC  ; <-- edit
    MOVWF 0x01
    
    ; # is negative (0: positive; 1: negative)
    CLRF 0x10
    BTFSC 0x00, 7
	BTG 0x10, 0  ; negative
    BTFSC 0x00, 7
	NEGF 0x00  ; negative
    ; positive
    BTFSC 0x01, 7
	BTG 0x10, 0  ; negative
    BTFSC 0x01, 7
	NEGF 0x01  ; negative
    ; positive
    
    
    ; # mul
    CLRF 0x02
    CLRF 0x12  ; count
    mul_loop:
	MOVLW 4
	CPFSLT 0x12
	    GOTO end_mul_loop  ; count >= 4
	; count < 4
	
	; mul
	MOVF 0x00, 0
	BTFSC 0x01, 0
	    ADDWF 0x02  ; 1
	; 0
	
	; next run
	RLNCF 0x00
	RRNCF 0x01
	INCF 0x12
	GOTO mul_loop
    end_mul_loop:
    
    ; # negative or not
    BTFSC 0x10, 0
	NEGF 0x02  ; 1 -> negative
    ; 0 -> positive
end
