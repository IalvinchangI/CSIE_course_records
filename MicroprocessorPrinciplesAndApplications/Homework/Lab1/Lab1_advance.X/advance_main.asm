List p=18f4520
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	
	; # setup
	; x
	MOVLW b'11111111'
;	MOVLW b'01101000'
	MOVWF 0x00
	; clz = 0
	MOVLW 0x00
	MOVWF 0x10
	
	; temp = 8
	MOVLW 0x08
	
	; # count leading zeros (CLZ) value
	loop:
	    CPFSLT 0x10  ; clz < 8 => skip
		GOTO end_loop  ; when clz >= 8
	    ; check MSB
	    BTFSC 0x00, 7  ; x[7] == 0 => skip
		GOTO end_loop  ; when x[7] = 1
	    ; continue
	    INCF 0x10
	    RLNCF 0x00
	    GOTO loop
	end_loop:

end
