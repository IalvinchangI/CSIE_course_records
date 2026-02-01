#include "xc.inc"
GLOBAL _is_prime
PSECT mytext, local, class=CODE, reloc=2
 
_is_prime:
    ; save input
    MOVWF 0x10
    
    ; init counter
    MOVLW 0x01
    MOVWF 0x11  ; counter
    
    ; 1 is not prime
    CPFSEQ 0x10
	GOTO normal
    GOTO output_F
    
    ; 2 ~ input
    normal:
    next_number:
	INCF 0x11
	MOVF 0x11, W  ; temp = WREG = counter
	CPFSEQ 0x10
	    GOTO increase
	GOTO output_T
	increase:
	    ADDWF 0x11, W
	    BC next_number  ; WREG > input
	    CPFSLT 0x10
		GOTO greater_equal  ; WREG <= input
	    GOTO next_number  ; input < WREG
	    greater_equal:
	    CPFSEQ 0x10
		GOTO increase  ; WREG < input
	    GOTO output_F  ; input = counter * q = WREG
    
    ; output
    output_T:
    MOVLW 0x01
    MOVFF WREG, 0x01
    RETURN
    output_F:
    MOVLW 0xFF
    MOVFF WREG, 0x01
    RETURN
