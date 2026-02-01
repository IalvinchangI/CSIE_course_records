;#include "xc.inc"
    
#include "Arithmetic16_macro.asm"
#include "Branch16_macro.asm"
#include "OtherTools16_macro.asm"

;PSECT arithmetic16, global, class=CODE, reloc=2
    
    ; x = (arithmetic_arg0_H, arithmetic_arg0_L)
    ; y = (arithmetic_arg1_H, arithmetic_arg1_L)
    ; return = (arithmetic_return_HH, arithmetic_return_HL, arithmetic_return_LH, arithmetic_return_LL)
    ; q = (arithmetic_return_HH, arithmetic_return_HL)
    ; r = (arithmetic_return_LH, arithmetic_return_LL)
    ; temp = (0x6?)
    
    ; ################################### MUL ###################################
    ; $ def 16b + 16b = 32b
    UnsignedMultiply16:  ; x * y = reuturn
	; init
	CLRF arithmetic_return_LL, A
	CLRF arithmetic_return_LH, A
	CLRF arithmetic_return_HL, A
	CLRF arithmetic_return_HH, A

	; # mul L, L
	MOVF arithmetic_arg0_L, W, A
	MULWF arithmetic_arg1_L, A
	movff16 PRODH, PRODL, arithmetic_return_LH, arithmetic_return_LL
	; # mul H, L
	MOVF arithmetic_arg0_H, W, A
	MULWF arithmetic_arg1_L, A
	add16c arithmetic_return_HL, arithmetic_return_LH, PRODH, PRODL, arithmetic_return_HH, arithmetic_return_HL, arithmetic_return_LH
	; # mul L, H
	MOVF arithmetic_arg0_L, W, A
	MULWF arithmetic_arg1_H, A
	add16c arithmetic_return_HL, arithmetic_return_LH, PRODH, PRODL, arithmetic_return_HH, arithmetic_return_HL, arithmetic_return_LH
	; # mul H, H
	MOVF arithmetic_arg0_H, W, A
	MULWF arithmetic_arg1_H, A
	add16 arithmetic_return_HH, arithmetic_return_HL, PRODH, PRODL, arithmetic_return_HH, arithmetic_return_HL

	RETURN

    ; $ def 16b + 16b = 32b
    Multiply16:  ; x * y = reuturn
	RCALL UnsignedMultiply16

	; a is negative
	BTFSS arithmetic_arg0_H, 7, A
	    BRA Multiply16_continue_check  ; non negative
	; sub b
	sub16 arithmetic_return_HH, arithmetic_return_HL, arithmetic_arg1_H, arithmetic_arg1_L, arithmetic_return_HH, arithmetic_return_HL

	Multiply16_continue_check:
	; b is negative
	BTFSS arithmetic_arg1_H, 7, A
	    BRA Multiply16_finish_check  ; non negative
	; sub a
	sub16 arithmetic_return_HH, arithmetic_return_HL, arithmetic_arg0_H, arithmetic_arg0_L, arithmetic_return_HH, arithmetic_return_HL

	Multiply16_finish_check:
	RETURN
	
	
    ; ################################### DIV ###################################
    ; $ def 16b \ 16b = 16b ... 16b
    ; WARNING: Won't Check Whether y = 0
    UnsignedDivide16:  ; x \ y = q ... r
	; init
	movff16 arithmetic_arg0_H, arithmetic_arg0_L, t2, t1  ; x
	movff16 arithmetic_arg1_H, arithmetic_arg1_L, t4, t3  ; y
	CLRF arithmetic_return_HL, A  ; counter L
	CLRF arithmetic_return_HH, A  ; counter H
	CLRF t5, A  ; shift_counter L
	INCF t5, F, A
	CLRF t6, A  ; shift_counter H
	CLRF t0, A  ; 0
	
	; x < y ? UnsignedDivide16_divide_end : UnsignedDivide16_shift_check
	less16 t2, t1, t4, t3, UnsignedDivide16_divide_end, UnsignedDivide16_shift_check
	
	; shift check
	UnsignedDivide16_shift_check:
	    shift_left16 t4, t3
	    CLRF t7, A  ; temp which is used to store MSB
	    MOVLW 0H
	    ADDWFC t7, F, A
	    BNZ UnsignedDivide16_shift_check_end  ; overflow
	    shift_left16 t6, t5  ; shift shift_counter
;	    ; over 16
;	    ; shift_counter == 0 ? UnsignedDivide16_shift_check_0end : UnsignedDivide16_shift_check_countinue
;	    equal16 t6, t5, t0, t0, UnsignedDivide16_shift_check_0end, UnsignedDivide16_shift_check_countinue
	    UnsignedDivide16_shift_check_countinue:
	    ; x < y ? UnsignedDivide16_shift_check_1end : UnsignedDivide16_shift_check
	    less16 t2, t1, t4, t3, UnsignedDivide16_shift_check_1end, UnsignedDivide16_shift_check
;	UnsignedDivide16_shift_check_0end:  ; recover t6, t5
;	BSF t6, 7, A
;	BRA UnsignedDivide16_shift_check_end
	UnsignedDivide16_shift_check_1end:  ; shift t6, t5
	shift_right16 t6, t5
	UnsignedDivide16_shift_check_end:  ; shift t4, t3
	shift_right16 t4, t3
	; put MSB back
	RRNCF t7, F, A
	MOVF t7, W, A
	IORWF t4, F, A
	; p.s. now, x >= y
	
	; divide
	UnsignedDivide16_divide:
	    sub16 t2, t1, t4, t3, t2, t1  ; x - y = x
	    add16 arithmetic_return_LH, arithmetic_return_LL, t6, t5, arithmetic_return_LH, arithmetic_return_LL  ; q + shift_counter = q
	    ; p.s. now, x < y
	    UnsignedDivide16_divide_shift:
		shift_right16 t4, t3
		shift_right16 t6, t5
		BC UnsignedDivide16_divide_end
;		; lower than 1
;		; shift_counter == 0 ? UnsignedDivide16_divide_end : UnsignedDivide16_divide_shift_countinue
;		equal16 t6, t5, t0, t0, UnsignedDivide16_divide_end, UnsignedDivide16_divide_shift_countinue
;		UnsignedDivide16_divide_shift_countinue:
		; x < y ? UnsignedDivide16_divide_shift : UnsignedDivide16_divide
		less16 t2, t1, t4, t3, UnsignedDivide16_divide_shift, UnsignedDivide16_divide
	UnsignedDivide16_divide_end:
	movff16 t2, t1, arithmetic_return_HH, arithmetic_return_HL  ; x -> r
	RETURN
    
    ; $ def 16b \ 16b = 16b ... 16b
    ; WARNING: Won't Check Whether y = 0
    Divide16:  ; x \ y = q ... r
	CLRF tA, A  ; store negative
	; check negative
	Divide16_check_x_negative:
	BTFSS arithmetic_arg0_H, 7, A
	    BRA Divide16_check_y_negative  ; non negative
	; negative
	xori16 arithmetic_arg0_H, arithmetic_arg0_L, 0xFF, 0xFF, arithmetic_arg0_H, arithmetic_arg0_L
	inc16 arithmetic_arg0_H, arithmetic_arg0_L
	BSF tA, 0, A
	Divide16_check_y_negative:
	BTFSS arithmetic_arg1_H, 7, A
	    BRA Divide16_check_negative_end  ; non negative
	; negative
	xori16 arithmetic_arg1_H, arithmetic_arg1_L, 0xFF, 0xFF, arithmetic_arg1_H, arithmetic_arg1_L
	inc16 arithmetic_arg1_H, arithmetic_arg1_L
	BSF tA, 4, A
	Divide16_check_negative_end:
	
	RCALL UnsignedDivide16
	
	; recover negative
	Divide16_recover_x_negative:
	BTFSS tA, 0, A
	    BRA Divide16_recover_y_negative  ; non negative
	; negative
	xori16 arithmetic_arg0_H, arithmetic_arg0_L, 0xFF, 0xFF, arithmetic_arg0_H, arithmetic_arg0_L
	inc16 arithmetic_arg0_H, arithmetic_arg0_L
	xori16 arithmetic_return_HH, arithmetic_return_HL, 0xFF, 0xFF, arithmetic_return_HH, arithmetic_return_HL  ; q
	xori16 arithmetic_return_LH, arithmetic_return_LL, 0xFF, 0xFF, arithmetic_return_LH, arithmetic_return_LL  ; r
	Divide16_recover_y_negative:
	BTFSS tA, 4, A
	    BRA Divide16_recover_negative_end  ; non negative
	; negative
	xori16 arithmetic_arg1_H, arithmetic_arg1_L, 0xFF, 0xFF, arithmetic_arg1_H, arithmetic_arg1_L
	inc16 arithmetic_arg1_H, arithmetic_arg1_L
	xori16 arithmetic_return_HH, arithmetic_return_HL, 0xFF, 0xFF, arithmetic_return_HH, arithmetic_return_HL  ; q
	Divide16_recover_negative_end:
	
	RETURN
	
    end
