List p=18f4520
    #include<p18f4520.inc>
        CONFIG OSC = INTIO67
        CONFIG WDT = OFF
        org 0x00
	
	; # setup
	; x
	MOVLW 0xFF
;	MOVLW b'01101000'
	MOVWF 0x00
	; clz = r = 0
	CLRF 0x10
	
	
	; # count leading zeros (CLZ) value
	; ## 1 / 2 (bit 4 -> 0x10)
	; if (x < 0x10) r += 4
	MOVLW 0x10
	CPFSGT 0x00  ; x > W = 0x10 => skip
	    BSF 0x10, 2  ; r += 4
	; if (r[2] == 1) x << 4
	BTFSC 0x10, 2  ; r[2] == 0 => skip
	    RLNCF 0x00
	BTFSC 0x10, 2  ; r[2] == 0 => skip
	    RLNCF 0x00
	BTFSC 0x10, 2  ; r[2] == 0 => skip
	    RLNCF 0x00
	BTFSC 0x10, 2  ; r[2] == 0 => skip
	    RLNCF 0x00
	
	; ## 3 / 4 (bit 6 -> 0x40)
	; if (x < 0x40) r += 2
	MOVLW 0x40
	CPFSGT 0x00  ; x > W = 0x40 => skip
	    BSF 0x10, 1  ; r += 2
	; if (r[1] == 1) x << 2
	BTFSC 0x10, 1  ; r[1] == 0 => skip
	    RLNCF 0x00
	BTFSC 0x10, 1  ; r[1] == 0 => skip
	    RLNCF 0x00
	
	; ## 7 / 8 (bit 7 -> 0x80)
	; if (x <= 0x7F) r += 1
	MOVLW 0x7F
	CPFSGT 0x00  ; x > W = 0x7F => skip
	    BSF 0x10, 0  ; r += 1
	; if (r[0] == 1) x << 1
	BTFSC 0x10, 0  ; r[0] == 0 => skip
	    RLNCF 0x00
	
	; ## test x == 0 ?
	; if (x == 0) r += 1
	MOVLW 0x00
	CPFSGT 0x00  ; x > W = 0x00 => skip
	    INCF 0x10  ; r += 1

end
