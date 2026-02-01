List p=18f4520
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	
	; # setup
	; X1
	MOVLW 0xB6
	MOVWF 0x00
	; X2
	MOVLW 0x0C
	MOVWF 0x01
	; Y1
	MOVLW 0xD3
	MOVWF 0x02
	; Y2
	MOVLW 0xB7
	MOVWF 0x03
	
	; # compute
	; A1   = X1   + X2
	; 0x10 = 0x00 + 0x01
	MOVF 0x00, 0
	ADDWF 0x01, 0
	MOVWF 0x10
	
	; A2   = Y1   - Y2
	; 0x11 = 0x02 + 0x03
	MOVF 0x03, 0
	SUBWF 0x02, 0  ;F - W
	MOVWF 0x11
	; W = 0x11 = A2
	
	; # Compare
	; A1 >  A2 => 0x20 = 0xFF
	; A1 <= A2 => 0x20 = 0x01
	CPFSGT 0x10  ; A1 > W = A2 => skip
	    GOTO compare_else
	; A1 > A2
	MOVLW 0xFF  ; W = 0xFF
	GOTO end_compare_if
	
	compare_else:  ; A1 <= A2
	MOVLW 0x01  ; W = 0x01
	
	end_compare_if:  ; store
	MOVWF 0x20  ; 0x20 = W

end
