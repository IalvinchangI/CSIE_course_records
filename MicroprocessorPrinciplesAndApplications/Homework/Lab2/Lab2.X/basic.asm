#INCLUDE <p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00    ;PC = 0x00 
    
    ; # setup
    ; pointer
    LFSR 0, 0x120
    LFSR 1, 0x121
    LFSR 2, 0x122
    ; data
    MOVLW 0x8E  ; <-- edit
    MOVWF INDF0
    MOVLW 0x37  ; <-- edit
    MOVWF INDF1
    
    ; # loop
    loop:
	; end loop check
	MOVLW 0x27  ; end = 0x26 + 1
	CPFSLT FSR2L
	    GOTO end_loop  ; pointer2 >= end
	 
	MOVF POSTINC1, W
	; operate
	BTFSS FSR2L, 0  ; even -> add
	    ADDWF POSTINC0, W
	BTFSC FSR2L, 0  ; odd -> sub
	    SUBWF POSTINC0, W
	; operate/
	MOVWF POSTINC2
	GOTO loop
    end_loop:
    
end
