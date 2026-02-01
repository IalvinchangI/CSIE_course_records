#INCLUDE <p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00    ;PC = 0x00 
    
    And_Mul macro xh, xl, yh, yl
	; # setup
	; data x
	MOVLW xh
	MOVWF 0x02
	MOVLW xl
	MOVWF 0x03
	; data y
	MOVLW yh
	MOVWF 0x04
	MOVLW yl
	MOVWF 0x05
	
	; # And H
	MOVF 0x02, W
	ANDWF 0x04, W
	MOVWF 0x00
	; # And L
	MOVF 0x03, W
	ANDWF 0x05, W
	MOVWF 0x01
	
	; # mul
	MOVF 0x00, W
	MULWF 0x01
	MOVFF PRODH, 0x10
	MOVFF PRODL, 0x11
	; H is negative
	MOVF 0x00, W
	BTFSC 0x00, 7
	    SUBWF 0x10, F
	; L is negative
	MOVF 0x01, W
	BTFSC 0x01, 7
	    SUBWF 0x10, F
    endm

    ; # exec
    And_Mul 0x50, 0x6F, 0x3A, 0xBC
;    And_Mul 0x56, 0xAB, 0x25, 0x32
    
end
