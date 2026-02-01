#INCLUDE <p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00    ;PC = 0x00 

    ; # setup
    ; pointer
    LFSR 0, 0x200  ; A
    LFSR 1, 0x210  ; B
    LFSR 2, 0x220  ; output
    ; sequence A
    MOVLW 0x00  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0x33  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0x58  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0x7A  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0xC4  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0xF0  ; <-- edit
    MOVWF POSTINC0
    LFSR 0, 0x200  ; reset A
    ; sequence B
    MOVLW 0x09  ; <-- edit
    MOVWF POSTINC1
    MOVLW 0x58  ; <-- edit
    MOVWF POSTINC1
    MOVLW 0x6E  ; <-- edit
    MOVWF POSTINC1
    MOVLW 0xB8  ; <-- edit
    MOVWF POSTINC1
    MOVLW 0xDD  ; <-- edit
    MOVWF POSTINC1
    LFSR 1, 0x210  ; reset B
    
    ; # merge
    loop:
	; end loop check
	MOVLW 0x2B  ; end = 0x2A + 1
	CPFSLT FSR2L
	    GOTO end_loop  ; pointer2 >= end
	; sequence A finish
	MOVLW 0x06  ; end = 0x05 + 1
	CPFSLT FSR0L
	    GOTO put_B  ; pointer0 >= end
	; sequence B finish
	MOVLW 0x15  ; end = 0x14 + 1
	CPFSLT FSR1L
	    GOTO put_A  ; pointer1 >= end
	
	; compare
	MOVF INDF1, W
	CPFSGT INDF0
	    GOTO put_A  ; A <= B
	GOTO put_B  ; A > B
	
	; operate
	put_A:
	    MOVFF POSTINC0, POSTINC2
	    GOTO loop
	put_B:
	    MOVFF POSTINC1, POSTINC2
	    GOTO loop
    end_loop:
end
