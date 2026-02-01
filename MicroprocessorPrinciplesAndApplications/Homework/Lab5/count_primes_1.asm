#include "xc.inc"
GLOBAL _count_primes
PSECT mytext, local, class=CODE, reloc=2


; $ def 16b + 1 = 16b
inc16 macro xh, xl  ; x + 1 = x
    ; inc L
    INCF xl, a
    ; inc H
    MOVLW 0x00
    ADDWFC xh, F, a
endm

; $ def 16b + 16b = 16b
add16 macro xh, xl, yh, yl, ah, al  ; x + y = a
    ; add L
    MOVF yl, W, a
    ADDWF xl, W, a
    MOVWF al, a
    ; add H
    MOVF yh, W, a
    ADDWFC xh, W, a
    MOVWF ah, a
endm

; $ def 16b -> 16b
movff16 macro xh, xl, yh, yl  ; x -> y
    MOVFF xl, yl, a
    MOVFF xh, yh, a
endm

; $ def 16b >> 1 = 16b
shift_right16 macro xh, xl  ; x >> 1 = x
    ; shift right H
    RRCF xh
    ; shift right L
    RRCF xl
    endm

; $ def 16b << 1 = 16b
shift_left16 macro xh, xl  ; x << 1 = x
    ; shift left L
    RLCF xl, a
    BSF xl, 0, a
    ; shift right H
    RRCF xh
endm

; $ def 16b / 16b = 16b ... 16b
div16 macro xh, xl, yh, yl, qh, ql, rh, rl  ; x / y = q ... r
    CLRF 0x30  ; shift counter
    CLRF 0x32
    CLRF 0x33
    CLRF 0x34
    CLRF 0x35
    
    shift_left16 yh, yl
    INCF 0x30
    BNC -3
    shift_right16 yh, yl
    DECF 0x30
    
    
    
endm

; $ def 16b == 16b ? addr1 : addr2
equal16 macro xh, xl, yh, yl, addr1, addr2  ; x == y ? addr1 : addr2
    ; equal L
    MOVF yl, W, a
    CPFSEQ xl, a
	GOTO addr2  ; not_equal
    ; equal H
    MOVF yh, W, a
    CPFSEQ xh
	GOTO addr2  ; not_equal
    GOTO addr1  ; equal
endm
    
; $ def 16b < 16b ? addr1 : addr2
; p.s. 0x20 must be 0
less16 macro xh, xl, yh, yl, addr1, addr2  ; x < y ? addr1 : addr2
    ; less H
    MOVF yh, W, a
    CPFSGT xh
	TSTFSZ 0x20  ; GOTO __less16_lessH  ; xh <= yh
    GOTO addr2  ; xh > yh
    ; __less16_lessH:
    CPFSEQ xh
	GOTO addr1  ; xh < yh
    ; xh == yh
    ; less L
    MOVF yl, W, a
    CPFSLT xl
	GOTO addr2  ; xl >= yl
    GOTO addr1  ; xl < yl
endm
    
; $ def 16b, 16b -> T/F
is_prime:  ; L: 0x10, H: 0x11
    ; init counter
    MOVLW 0x01
    MOVWF 0x12, a  ; counter L
    CLRF 0x13, a  ; counter H
    
    ; 1 is not prime
    equal16 0x13, 0x12, 0x11, 0x10, output_F, normal
    ; 2 is prime
    equal16 0x20, 0x21, 0x11, 0x10, output_T, normal
    BTFSS 0x10, 0, a
	GOTO output_F
    
    ; 3 ~ input
    normal:
    next_number:
	add16 0x13, 0x12, 0x20, 0x21, 0x13, 0x12  ; counter += 2
	movff16 0x13, 0x12, 0x23, 0x22  ; temp = (0x23, 0x22) = counter
	equal16 0x23, 0x22, 0x11, 0x10, output_T, increase  ; input == counter or not
	increase:
	    add16 0x13, 0x12, 0x23, 0x22, 0x23, 0x22
	    BC next_number  ; temp > input
	    less16 0x11, 0x10, 0x23, 0x22, next_number, greater_equal
	    ; next_number: input < temp
	    ; greater_equal: input >= temp
	    greater_equal:
	    equal16 0x23, 0x22, 0x11, 0x10, output_F, increase  ; input == temp or not
	    ; output_F: input = counter * q = temp
	    ; increase: temp < input
    
    ; output
    output_T:
    MOVLW 0x01
    RETURN
    output_F:
    MOVLW 0x00
    RETURN

    
_count_primes:
    BCF WDTCON, 0
    CLRF 0x20, a  ; 0
    MOVLW 0x02
    MOVWF 0x21, a  ; 2
    movff16 0x02, 0x01, 0x11, 0x10
    ; counter = (0x02, 0x01)
    CLRF 0x01, a  ; L
    CLRF 0x02, a  ; H
    loop:
	RCALL is_prime
	add16 0x02, 0x01, 0x20, WREG, 0x02, 0x01
	; next loop
	inc16 0x11, 0x10
	BC end_loop  ; overflow
	less16 0x04, 0x03, 0x11, 0x10, end_loop, loop  ; b < a
    end_loop:
    RETURN
