#INCLUDE <p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00    ;PC = 0x00 

    ; # setup
    ; data A
    MOVLW 0x9A  ; <-- edit
    MOVWF 0x00
    MOVLW 0xBC  ; <-- edit
    MOVWF 0x01
    ; data B
    MOVLW 0x12  ; <-- edit
    MOVWF 0x10
    MOVLW 0x34  ; <-- edit
    MOVWF 0x11
    
    ; # -B
    COMF 0x10
    COMF 0x11
    INCF 0x11
    BNC complement_B_no_carry
	INCF 0x10
    complement_B_no_carry:
    
    ; # A + -B
    MOVF 0x01, 0
    ADDWF 0x11, 0
    BNC add_lower_bit_no_carry
	INCF 0x00
    add_lower_bit_no_carry:
    MOVWF 0x21
    
    MOVF 0x00, 0
    ADDWF 0x10, 0
    MOVWF 0x20
    
end
