#INCLUDE <p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00    ;PC = 0x00 

    ; # setup
    ; data
    MOVLW 0xA6  ; <-- edit
    MOVWF TRISA
    
    
    ; # reverse
    MOVFF TRISA, 0x00
    CLRF TRISA
    
    ; 0
    MOVLW 0x01
    ANDWF 0x00, 0
    IORWF TRISA, 1
    RRNCF 0x00
    RLNCF TRISA
    ; 1
    MOVLW 0x01
    ANDWF 0x00, 0
    IORWF TRISA, 1
    RRNCF 0x00
    RLNCF TRISA
    ; 2
    MOVLW 0x01
    ANDWF 0x00, 0
    IORWF TRISA, 1
    RRNCF 0x00
    RLNCF TRISA
    ; 3
    MOVLW 0x01
    ANDWF 0x00, 0
    IORWF TRISA, 1
    RRNCF 0x00
    RLNCF TRISA
    ; 4
    MOVLW 0x01
    ANDWF 0x00, 0
    IORWF TRISA, 1
    RRNCF 0x00
    RLNCF TRISA
    ; 5
    MOVLW 0x01
    ANDWF 0x00, 0
    IORWF TRISA, 1
    RRNCF 0x00
    RLNCF TRISA
    ; 6
    MOVLW 0x01
    ANDWF 0x00, 0
    IORWF TRISA, 1
    RRNCF 0x00
    RLNCF TRISA
    ; 7
    MOVLW 0x01
    ANDWF 0x00, 0
    IORWF TRISA, 1
    
end
