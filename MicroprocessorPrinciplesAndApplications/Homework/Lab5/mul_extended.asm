#include "xc.inc"
GLOBAL _mul_extended
PSECT mytext, local, class=CODE, reloc=2

; $ def 16b -> 16b
movff16 macro xh, xl, yh, yl  ; x -> y
    MOVFF xl, yl, a
    MOVFF xh, yh, a
endm

; $ def 16b + 16b = 16b + c
add16c macro xh, xl, yh, yl, ac, ah, al  ; x + y = a
    ; add L
    MOVF yl, W, a
    ADDWF xl, W, a
    MOVWF al, a
    ; add H
    MOVF yh, W, a
    ADDWFC xh, W, a
    MOVWF ah, a
    ; add c
    MOVLW 0x00
    ADDWFC ac, F, a
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


_mul_extended:
    ; init
    movff16 0x02, 0x01, 0x12, 0x11  ; a
    movff16 0x04, 0x03, 0x14, 0x13  ; b
    CLRF 0x01, a
    CLRF 0x02, a
    CLRF 0x03, a
    CLRF 0x04, a
    
    ; # mul L, L
    MOVF 0x11, W
    MULWF 0x13
    MOVFF PRODH, 0x02
    MOVFF PRODL, 0x01
    ; # mul H, L
    MOVF 0x12, W
    MULWF 0x13
    add16c 0x03, 0x02, PRODH, PRODL, 0x04, 0x03, 0x02
    ; # mul L, H
    MOVF 0x11, W
    MULWF 0x14
    add16c 0x03, 0x02, PRODH, PRODL, 0x04, 0x03, 0x02
    ; # mul H, H
    MOVF 0x12, W
    MULWF 0x14
    add16c 0x04, 0x03, PRODH, PRODL, 0x05, 0x04, 0x03
    
    ; a is negative
    BTFSS 0x12, 7
	GOTO continue_check
    ; sub b
    sub16 0x04, 0x03, 0x14, 0x13, 0x04, 0x03
    
    continue_check:
    ; b is negative
    BTFSS 0x14, 7
	GOTO finish_check
    ; sub a
    sub16 0x04, 0x03, 0x12, 0x11, 0x04, 0x03
    
    finish_check:
    RETURN
