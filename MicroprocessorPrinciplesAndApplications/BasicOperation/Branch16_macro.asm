#ifndef __branch16_macro_
#define __branch16_macro_
;    #include "xc.inc"
    
    ; ################################### EQUAL ###################################
    ; $ def 16b == 16b ? addr1 : addr2
    equal16 macro xh, xl, yh, yl, addr1, addr2  ; x == y ? addr1 : addr2
	; equal L
	MOVF yl, W, A
	CPFSEQ xl, A
	    GOTO addr2  ; not_equal
	; equal H
	MOVF yh, W, A
	CPFSEQ xh, A
	    GOTO addr2  ; not_equal
	GOTO addr1  ; equal
    endm
    
    
    ; ################################### LESS ###################################
    ; $ def 16b < 16b ? addr1 : addr2
    less16 macro xh, xl, yh, yl, addr1, addr2  ; x < y ? addr1 : addr2
	LOCAL __less16_lessH
	; less H
	MOVF yh, W, A
	CPFSGT xh, A
	    BRA __less16_lessH  ; GOTO __less16_lessH  ; xh <= yh
	GOTO addr2  ; xh > yh
	__less16_lessH:
	CPFSEQ xh, A
	    GOTO addr1  ; xh < yh
	; xh == yh
	; less L
	MOVF yl, W, A
	CPFSLT xl, A
	    GOTO addr2  ; xl >= yl
	GOTO addr1  ; xl < yl
    endm
    
    
    ; ################################### LESS & EQUAL ###################################
    ; $ def 16b <= 16b ? addr1 : addr2
    less_equal16 macro xh, xl, yh, yl, addr1, addr2  ; x <= y ? addr1 : addr2
	LOCAL __less16_lessH
	; less H
	MOVF yh, W, A
	CPFSGT xh, A
	    BRA __less16_lessH  ; GOTO __less16_lessH  ; xh <= yh
	GOTO addr2  ; xh > yh
	__less16_lessH:
	CPFSEQ xh, A
	    GOTO addr1  ; xh < yh
	; # xh == yh
	; less L
	MOVF yl, W, A
	CPFSGT xl, A
	    GOTO addr1  ; xl <= yl
	GOTO addr2  ; xl > yl
    endm

#endif
