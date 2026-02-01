;#include "xc.inc"

#INCLUDE <p18f4520.inc>
CONFIG OSC = INTIO67
CONFIG WDT = OFF
org 0x00    ;PC = 0x00 

arg0_H EQU 0x02
arg0_L EQU 0x01
arg1_H EQU 0x04
arg1_L EQU 0x03
return_HH EQU 0x14
return_HL EQU 0x13
return_LH EQU 0x12
return_LL EQU 0x11
    
#include "Arithmetic16_macro.asm"
#include "Branch16_macro.asm"
#include "OtherTools16_macro.asm"

;GLOBAL _test_function
;GLOBAL _unsigned_test_function
;PSECT test_function, local, class=CODE, reloc=2
    
    GOTO my_main
    _test_function:
    _unsigned_test_function:
;	inc16 arg0_H, arg0_L
;	add16 arg0_H, arg0_L, arg1_H, arg1_L, return_LH, return_LL
;	add16c arg0_H, arg0_L, arg1_H, arg1_L, return_HL, return_LH, return_LL
;	addi16c arg0_H, arg0_L, 0xF5, 0x21, return_HL, return_LH, return_LL
	
;	dec16 arg0_H, arg0_L
;	sub16 arg0_H, arg0_L, arg1_H, arg1_L, return_LH, return_LL
;	subi16 arg0_H, arg0_L, 0xF6, 0x20, return_LH, return_LL
;	subi16_inverse 0x64, 0x00, arg1_H, arg1_L, return_LH, return_LL
	
;	and16 arg0_H, arg0_L, arg1_H, arg1_L, return_LH, return_LL
;	andi16 arg0_H, arg0_L, 0x07, 0x20, return_LH, return_LL
;	or16 arg0_H, arg0_L, arg1_H, arg1_L, return_LH, return_LL
;	ori16 arg0_H, arg0_L, 0x07, 0x20, return_LH, return_LL
;	xor16 arg0_H, arg0_L, arg1_H, arg1_L, return_LH, return_LL
;	xori16 arg0_H, arg0_L, 0x07, 0x20, return_LH, return_LL
    
;	shift_right16 arg0_H, arg0_L
;	rotate_left16c arg0_H, arg0_L
;	rotate_left16c arg0_H, arg0_L
;	shift_right16 arg0_H, arg0_L
;	rotate_right16c arg0_H, arg0_L
;	rotate_right16c arg0_H, arg0_L
;	shift_right16 arg0_H, arg0_L
;	rotate_right16c arg0_H, arg0_L
;	rotate_right16c arg0_H, arg0_L
;	shift_left16 arg0_H, arg0_L
;	shift_left16 arg0_H, arg0_L
	
;	equal16 arg0_H, arg0_L, arg1_H, arg1_L, addr1, addr2
;	less16 arg0_H, arg0_L, arg1_H, arg1_L, addr1, addr2
;	less_equal16 arg0_H, arg0_L, arg1_H, arg1_L, addr1, addr2
;	SETF return_LH, A
;	addr1:
;	    SETF return_HL, A
;	    RETURN
;	addr2:
;	    SETF return_HH, A
;	    RETURN
	
;	movff16 arg0_H, arg0_L, arithmetic_arg0_H, arithmetic_arg0_L
;	movff16 arg1_H, arg1_L, arithmetic_arg1_H, arithmetic_arg1_L
;	RCALL UnsignedMultiply16
;	RCALL Multiply16
;	RCALL UnsignedDivide16
;	RCALL Divide16
;	movff16 arithmetic_return_HH, arithmetic_return_HL, return_HH, return_HL
;	movff16 arithmetic_return_LH, arithmetic_return_LL, return_LH, return_LL
	RETURN
	
    my_main:
	movif16 0x81, 0xB6, arg0_H, arg0_L
	movif16 0x00, 0x0f, arg1_H, arg1_L
	RCALL _test_function
	NOP

    #include "Arithmetic16.asm"
end
