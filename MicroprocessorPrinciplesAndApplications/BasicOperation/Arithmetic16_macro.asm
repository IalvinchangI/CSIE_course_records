#ifndef __arithmetic16_macro_
#define __arithmetic16_macro_
;    #include "xc.inc"
    
    arithmetic_arg0_H EQU 0x52  ; xh
    arithmetic_arg0_L EQU 0x51  ; xl
    arithmetic_arg1_H EQU 0x54  ; yh
    arithmetic_arg1_L EQU 0x53  ; yl
    arithmetic_return_HH EQU 0x58
    arithmetic_return_HL EQU 0x57
    arithmetic_return_LH EQU 0x56
    arithmetic_return_LL EQU 0x55
    
    ; ################################### ADD ###################################
    ; $ def 16b + 1 = 16b
    inc16 macro xh, xl  ; x + 1 = x
	; inc L
	INCF xl, F, A
	; inc H
	MOVLW 0x00
	ADDWFC xh, F, A
    endm
    
    ; $ def 16b + 16b = 16b
    add16 macro xh, xl, yh, yl, ah, al  ; x + y = a
	; add L
	MOVF yl, W, A
	ADDWF xl, W, A
	MOVWF al, A
	; add H
	MOVF yh, W, A
	ADDWFC xh, W, A
	MOVWF ah, A
    endm
    ; $ def 16b + 16b = 16b + carry
    add16c macro xh, xl, yh, yl, ac, ah, al  ; x + y = a
	add16 xh, xl, yh, yl, ah, al
	; add c
	MOVLW 0x00
	ADDWFC ac, F, A
    endm

    ; $ def 16b + 16b immediate = 16b
    addi16 macro xh, xl, ih, il, ah, al  ; x + i = a
	; add L
	MOVLW il
	ADDWF xl, W, A
	MOVWF al, A
	; add H
	MOVLW ih
	ADDWFC xh, W, A
	MOVWF ah, A
    endm
    ; $ def 16b + 16b immediate = 16b + carry
    addi16c macro xh, xl, ih, il, ac, ah, al  ; x + i = a
	addi16 xh, xl, ih, il, ah, al
	; add c
	MOVLW 0x00
	ADDWFC ac, F, A
    endm
    
    
    ; ################################### SUB ###################################
    ; $ def 16b - 1 = 16b
    dec16 macro xh, xl  ; x - 1 = x
	; inc L
	DECF xl, F, A
	; inc H
	MOVLW 0x00
	SUBWFB xh, F, A
    endm
    
    ; $ def 16b - 16b = 16b
    sub16 macro xh, xl, yh, yl, ah, al  ; x - y = a
	; sub L
	MOVF yl, W, A
	SUBWF xl, W, A  ; F - W
	MOVWF al, A
	; sub H
	MOVF yh, W, A
	SUBWFB xh, W, A  ; F - W
	MOVWF ah, A
    endm
    
    ; $ def 16b - 16b immediate = 16b
    subi16 macro xh, xl, ih, il, ah, al  ; x - i = a
	; sub L
	MOVLW il
	SUBWF xl, W, A  ; F - W
	MOVWF al, A
	; sub H
	MOVLW ih
	SUBWFB xh, W, A  ; F - W
	MOVWF ah, A
    endm
    ; $ def 16b immediate - 16b = 16b
    subi16_inverse macro ih, il, xh, xl, ah, al  ; i - x = a
	; sub L
	MOVF xl, W
	SUBLW il  ; i - W
	MOVWF al, A
	; sub H
	MOVLW ih
	SUBFWB xh, W, A  ; W - F
	MOVWF ah, A
    endm
    
    
    ; ################################### LOGIC ###################################
    ; AND
    ; $ def 16b & 16b = 16b
    and16 macro xh, xl, yh, yl, ah, al  ; x & y = a
	; and L
	MOVF yl, W, A
	ANDWF xl, W, A
	MOVWF al, A
	; and H
	MOVF yh, W, A
	ANDWF xh, W, A
	MOVWF ah, A
    endm
    ; $ def 16b & 16b immediate = 16b
    andi16 macro xh, xl, ih, il, ah, al  ; x & i = a
	; and L
	MOVLW il
	ANDWF xl, W, A
	MOVWF al, A
	; and H
	MOVLW ih
	ANDWF xh, W, A
	MOVWF ah, A
    endm

    ; OR
    ; $ def 16b | 16b = 16b
    or16 macro xh, xl, yh, yl, ah, al  ; x | y = a
	; or L
	MOVF yl, W, A
	IORWF xl, W, A
	MOVWF al, A
	; or H
	MOVF yh, W, A
	IORWF xh, W, A
	MOVWF ah, A
    endm
    ; $ def 16b | 16b immediate = 16b
    ori16 macro xh, xl, ih, il, ah, al  ; x | i = a
	; or L
	MOVLW il
	IORWF xl, W, A
	MOVWF al, A
	; or H
	MOVLW ih
	IORWF xh, W, A
	MOVWF ah, A
    endm

    ; XOR
    ; $ def 16b ^ 16b = 16b
    xor16 macro xh, xl, yh, yl, ah, al  ; x ^ y = a
	; xor L
	MOVF yl, W, A
	XORWF xl, W, A
	MOVWF al, A
	; xor H
	MOVF yh, W, A
	XORWF xh, W, A
	MOVWF ah, A
    endm
    ; $ def 16b ^ 16b immediate = 16b
    xori16 macro xh, xl, ih, il, ah, al  ; x ^ i = a
	; xor L
	MOVLW il
	XORWF xl, W, A
	MOVWF al, A
	; xor H
	MOVLW ih
	XORWF xh, W, A
	MOVWF ah, A
    endm
    
    ; SHIFT
    ; $ def 16b >> 1 = 16b
    shift_right16 macro xh, xl  ; x >> 1 = x
	; shift right H
	RRCF xh, F, A
	BCF  xh, 7, A
	; shift right L
	RRCF xl, F, A
    endm
    ; $ def 16b >> 1 = 16b with carry
    rotate_right16c macro xh, xl  ; x >> 1 = x
	; shift right H
	RRCF xh, F, A
	; shift right L
	RRCF xl, F, A
    endm
    ; $ def 16b << 1 = 16b
    shift_left16 macro xh, xl  ; x << 1 = x
	; shift left L
	RLCF xl, F, A
	BCF  xl, 0, A
	; shift left H
	RLCF xh, F, A
    endm
    ; $ def 16b << 1 = 16b with carry
    rotate_left16c macro xh, xl  ; x << 1 = x
	; shift left L
	RLCF xl, F, A
	; shift left H
	RLCF xh, F, A
    endm

#endif
