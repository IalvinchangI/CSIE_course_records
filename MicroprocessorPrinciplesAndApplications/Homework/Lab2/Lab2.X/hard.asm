#INCLUDE <p18f4520.inc>
    CONFIG OSC = INTIO67
    CONFIG WDT = OFF
    org 0x00    ;PC = 0x00 
    
    ; # setup
    ; pointer
    LFSR 0, 0x300  ; input
    LFSR 1, 0x310  ; has inverse (A)
    ; input sequence
    MOVLW 0xC4  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0xBB  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0xBB  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0x00  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0x4C  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0x8B  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0xBB  ; <-- edit
    MOVWF POSTINC0
    MOVLW 0x00  ; <-- edit
    MOVWF POSTINC0
    LFSR 0, 0x300  ; reset
    
    ; # find inverse
    MOVLW 0x10
    MOVWF 0x00  ; count
    ; init checked_map
	LFSR 2, 0x308  ; checked_map
	CLRF POSTINC2
	CLRF POSTINC2
	CLRF POSTINC2
	CLRF POSTINC2
	CLRF POSTINC2
	CLRF POSTINC2
	CLRF POSTINC2
	CLRF POSTINC2
    outer_loop:
	; end loop check
	MOVLW 0x08  ; end = 0x07 + 1
	CPFSLT FSR0L
	    GOTO end_outer_loop  ; pointer0 >= end
	    
	; init inner
	MOVFF FSR0L, FSR2L  ; pointer2 = pointer0
	; check checked_map
	    BSF FSR2L, 3
	    TSTFSZ INDF2
		GOTO next_element  ; has checked
	    ; not checked
	    BCF FSR2L, 3
	; start inner
	SWAPF POSTINC2  ;  SWAP it and pointer2++
	inner_loop:
	    ; end loop check
	    MOVLW 0x08  ; end = 0x07 + 1
	    CPFSLT FSR2L
		GOTO next_element  ; pointer2 >= end
	    ; check checked_map
	    BSF FSR2L, 3
	    TSTFSZ INDF2
		GOTO not_store  ; has checked
	    ; not checked
	    BCF FSR2L, 3
	    
	    ; compare
	    MOVF INDF2, W
	    CPFSEQ INDF0
		GOTO not_store  ; *pointer0 != *pointer2
	    GOTO store  ; *pointer0 == *pointer2
	    not_store:
	    BCF FSR2L, 3
	    INCF FSR2L
	    GOTO inner_loop
	; end_inner_loop
	
	store:
	    ; set checked_map
	    BSF FSR2L, 3
	    SETF INDF2
	    
	    ; find small one
	    SWAPF INDF0  ; pointer0: not SWAP; W: SWAP
	    CPFSGT INDF0
		MOVF INDF0, W  ; *pointer0 <= W
	    MOVWF 0x01  ; target
	    
	    ; insert small one into A
	    insert_loop:
		; end loop check
		MOVF 0x00, W  ; end = count
		CPFSLT FSR1L
		    GOTO end_insert_loop  ; pointer2 >= end
		; compare
		MOVF INDF1, W  ; temp = *pointer1 = current
		CPFSLT 0x01  ; compare target and current
		    GOTO not_swap_T_W  ; target >= current
		; target < current
		MOVFF 0x01, INDF1  ; *pointer1 = targe
		MOVWF 0x01  ; targe = temp
		not_swap_T_W: 
		    INCF FSR1L
		GOTO insert_loop
	    end_insert_loop:
	    MOVFF 0x01, INDF1
	    LFSR 1, 0x310  ; reset A
	    ; count++
	    INCF 0x00
	    GOTO next_element
	; end_store;
	next_element:
	    INCF FSR0L
	    GOTO outer_loop
    end_outer_loop:
    
    ; # legal?
    MOVLW 0x14  ; legal count
    CPFSEQ 0x00
	GOTO change_A  ; count != 0x14
    GOTO output  ; count == 0x14
    change_A:
    SETF POSTINC1
    SETF POSTINC1
    SETF POSTINC1
    SETF POSTINC1
    
    ; # output
    output:
    LFSR 2, 0x320  ; output
    LFSR 1, 0x30F  ; A = 0x310 - 1
    ; first half
    MOVFF PREINC1, POSTINC2
    MOVFF PREINC1, POSTINC2
    MOVFF PREINC1, POSTINC2
    MOVFF PREINC1, POSTINC2
    ; second half
    SWAPF INDF1
    MOVFF POSTDEC1, POSTINC2
    SWAPF INDF1
    MOVFF POSTDEC1, POSTINC2
    SWAPF INDF1
    MOVFF POSTDEC1, POSTINC2
    SWAPF INDF1
    MOVFF POSTDEC1, POSTINC2
end
