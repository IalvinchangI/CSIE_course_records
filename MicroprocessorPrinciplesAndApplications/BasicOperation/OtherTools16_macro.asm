#ifndef __otherTools16_macro_
#define __otherTools16_macro_
;    #include "xc.inc"
    
    t0 EQU 0x60
    t1 EQU 0x61
    t2 EQU 0x62
    t3 EQU 0x63
    t4 EQU 0x64
    t5 EQU 0x65
    t6 EQU 0x66
    t7 EQU 0x67
    t8 EQU 0x68
    t9 EQU 0x69
    tA EQU 0x6A
    tB EQU 0x6B
    tC EQU 0x6C
    tD EQU 0x6D
    tE EQU 0x6E
    tF EQU 0x6F
    
    ; ################################### MOVFF ###################################
    ; $ def 16b -> 16b
    movff16 macro xh, xl, yh, yl  ; x -> y
	MOVFF xl, yl
	MOVFF xh, yh
    endm
    ; $ def 16b immediate -> 16b
    movif16 macro ih, il, xh, xl  ; i -> x
	MOVLW il
	MOVWF xl, A
	MOVLW ih
	MOVWF xh, A
    endm
    
#endif
