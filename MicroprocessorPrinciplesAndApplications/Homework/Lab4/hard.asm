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
    ; $ def 16b + 16b = 16b
    add16 macro xh, xl, yh, yl, ah, al  ; x + y = a
	; sub L
	MOVF yl, W
	ADDWF xl, W
	MOVWF al
	; sub H
	MOVF yh, W
	ADDWFC xh, W
	MOVWF ah
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
    ; $ def 16b >> 1 = 16b
    shift_right16 macro xh, xl  ; x >> 1 = x
	; shift right L
	RRNCF xl
	; shift right H
	RRNCF xh
	BTFSS xh, 7
	    BCF xl, 7  ; is 0
	BTFSC xh, 7
	    BSF xl, 7  ; is 1
	BCF  xh, 7
    endm
    ; $ def 16b ^ 16b = 16b
    xor16 macro xh, xl, yh, yl, ah, al  ; x ^ y = a
	; # xor L
	MOVF xl, W
	XORWF yl, W
	MOVWF al
	; # xor H
	MOVF xh, W
	XORWF yh, W
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
    
    ; $def 16b $ 16b = 16b
    newtonSqrt:  ; 20_21 $ 22_23 = 24_25
	; temp_x = 10_11
	MOVFF 0x22, 0x24
	MOVFF 0x23, 0x25
	newtonSqrt_loop:
	    MOVFF 0x20, 0x00  ; Nh
	    MOVFF 0x21, 0x01  ; Nl
	    MOVFF 0x24, 0x02  ; x0h
	    MOVFF 0x25, 0x03  ; x0l
	    ; formula
	    RCALL division
	    add16 0x24, 0x25, 0x10, 0x11, 0x10, 0x11
	    shift_right16 0x10, 0x11
	    ; end formula
	    ; test equal
	    xor16 0x24, 0x25, 0x10, 0x11, 0x14, 0x15
	    CLRF WREG
	    CPFSEQ 0x14
		GOTO not_equal
	    CPFSEQ 0x15
		GOTO not_equal
	    GOTO end_newtonSqrt_loop  ; equal
	    not_equal:
		MOVFF 0x10, 0x24
		MOVFF 0x11, 0x25
	    GOTO newtonSqrt_loop
	end_newtonSqrt_loop:
	RETURN
    
    main:  ; ############################# main ###############################
    ; # setup
    ; data N
    MOVLW 0x30  ; <-- edit
    MOVWF 0x20
    MOVLW 0x21  ; <-- edit
    MOVWF 0x21
    ; data x0
    MOVLW 0x26  ; <-- edit
    MOVWF 0x22
    MOVLW 0x5D  ; <-- edit
    MOVWF 0x23
    
    ; # exec
    RCALL newtonSqrt
    NOP
end
