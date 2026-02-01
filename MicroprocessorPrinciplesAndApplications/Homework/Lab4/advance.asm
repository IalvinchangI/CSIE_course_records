#INCLUDE <p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00    ;PC = 0x00 
    
    GOTO main
    ; $ def 16b + 1 = 16b
    inc16 macro xh, xl  ; x + 1 = x
	; inc L
	INCF xl
	; inc H
	MOVLW 0x00
	ADDWFC xh, F
    endm
    ; $ def 16b - 16b = 16b
    sub16 macro xh, xl, yh, yl, ah, al  ; x - y = a
	; sub L
	MOVF yl, W
	SUBWF xl, W  ; F - W
	MOVWF al
	; sub H
	MOVF yh, W
	SUBWFB xh, W  ; F - W
	MOVWF ah
    endm
	
    ; $ def 16b / 16b = 16b + 16b
    division:  ; 00_01 / 02_03 = 10_11 + 12_13
	CLRF 0x10
	CLRF 0x11
	MOVFF 0x00, 0x12
	MOVFF 0x01, 0x13
	div16_loop:
	    sub16 0x00, 0x01, 0x02, 0x03, 0x00, 0x01
	    BNC end_div16_loop
	    
	    inc16 0x10, 0x11
	    MOVFF 0x00, 0x12
	    MOVFF 0x01, 0x13
	    GOTO div16_loop
	end_div16_loop:
	RETURN
    
    
    main:  ; ############################# main ###############################
    ; # setup
    ; data A
    MOVLW 0xFA  ; <-- edit
    MOVWF 0x00
    MOVLW 0x9F  ; <-- edit
    MOVWF 0x01
    ; data B
    MOVLW 0x03  ; <-- edit
    MOVWF 0x02
    MOVLW 0x45  ; <-- edit
    MOVWF 0x03
    
    ; # exec
    RCALL division
    
end
