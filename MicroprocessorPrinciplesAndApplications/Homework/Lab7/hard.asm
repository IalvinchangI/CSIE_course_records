#include "p18f4520.inc"

; CONFIG1H
  CONFIG  OSC = INTIO67         ; Oscillator Selection bits (Internal oscillator block, port function on RA6 and RA7)
  CONFIG  FCMEN = OFF           ; Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
  CONFIG  IESO = OFF            ; Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

; CONFIG2L
  CONFIG  PWRT = OFF            ; Power-up Timer Enable bit (PWRT disabled)
  CONFIG  BOREN = SBORDIS       ; Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
  CONFIG  BORV = 3              ; Brown Out Reset Voltage bits (Minimum setting)

; CONFIG2H
  CONFIG  WDT = OFF             ; Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
  CONFIG  WDTPS = 32768         ; Watchdog Timer Postscale Select bits (1:32768)

; CONFIG3H
  CONFIG  CCP2MX = PORTC        ; CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
  CONFIG  PBADEN = ON           ; PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
  CONFIG  LPT1OSC = OFF         ; Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
  CONFIG  MCLRE = ON            ; MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

; CONFIG4L
  CONFIG  STVREN = ON           ; Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
  CONFIG  LVP = OFF             ; Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
  CONFIG  XINST = OFF           ; Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

; CONFIG5L
  CONFIG  CP0 = OFF             ; Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
  CONFIG  CP1 = OFF             ; Code Protection bit (Block 1 (002000-003FFFh) not code-protected)
  CONFIG  CP2 = OFF             ; Code Protection bit (Block 2 (004000-005FFFh) not code-protected)
  CONFIG  CP3 = OFF             ; Code Protection bit (Block 3 (006000-007FFFh) not code-protected)

; CONFIG5H
  CONFIG  CPB = OFF             ; Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
  CONFIG  CPD = OFF             ; Data EEPROM Code Protection bit (Data EEPROM not code-protected)

; CONFIG6L
  CONFIG  WRT0 = OFF            ; Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
  CONFIG  WRT1 = OFF            ; Write Protection bit (Block 1 (002000-003FFFh) not write-protected)
  CONFIG  WRT2 = OFF            ; Write Protection bit (Block 2 (004000-005FFFh) not write-protected)
  CONFIG  WRT3 = OFF            ; Write Protection bit (Block 3 (006000-007FFFh) not write-protected)

; CONFIG6H
  CONFIG  WRTC = OFF            ; Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
  CONFIG  WRTB = OFF            ; Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
  CONFIG  WRTD = OFF            ; Data EEPROM Write Protection bit (Data EEPROM not write-protected)

; CONFIG7L
  CONFIG  EBTR0 = OFF           ; Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR1 = OFF           ; Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR2 = OFF           ; Table Read Protection bit (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
  CONFIG  EBTR3 = OFF           ; Table Read Protection bit (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

; CONFIG7H
  CONFIG  EBTRB = OFF           ; Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks)


; instruction frequency = 1 MHz / 4 = 0.25 MHz
; instruction time = 1/0.25 = 4 us
; Total_cycles = 2 + (2 + 8 * num1 + 3) * num2 cycles
; num1 = 111, num2 = 70, Total_cycles = 62512 cycles
; Total_delay ~= Total_cycles * instruction time = 0.25 s
DELAY macro num1, num2
    local LOOP1         ; Inner loop
    local LOOP2         ; Outer loop
    
    ; 2 cycles
    MOVLW num2          ; Load num2 into WREG
    MOVWF L2            ; Store WREG value into L2
    
    ; Total_cycles for LOOP2 = 2 cycles
    LOOP2:
    MOVLW num1
    MOVWF L1  
    
    ; Total_cycles for LOOP1 = 8 cycles
    LOOP1:
    NOP                 ; busy waiting
    NOP
    NOP
    NOP
    NOP
    DECFSZ L1, 1        
    BRA LOOP1           ; BRA instruction spends 2 cycles
    
    ; 3 cycles
    DECFSZ L2, 1        ; Decrement L2, skip if zero
    BRA LOOP2           
endm


; # DEFINE address
counter EQU LATD
L1 EQU 0x14         ; Define L1 memory location (used by DELAY macro)
L2 EQU 0x15         ; Define L2 memory location (used by DELAY macro)

; # DEFINE const
INCREMENT EQU 0x20


org 0x00
    GOTO INIT


org 0x08
HIGH_ISR:		; High Priority Interrupt Subroutine Table
    GOTO BUTTON_ISR


org 0x18
LOW_ISR:		; Low Priority Interrupt Subroutine Table
    GOTO TIMER2_ISR


org 0x50
BUTTON_ISR:	; Button Interrupt Subroutine
    ; reset counter
    MOVLW 0x1F
    ANDWF counter, F
    
    ; change mode
    BTG counter, 4
    
    ; decide 1s or 0.25s
    BTFSS counter, 4
	MOVLW D'61'	      ; 0  ; now, it is 1s, so it should become 0.25s
    BTFSC counter, 4
	MOVLW D'244'	      ; 1  ; now, it is 0.25s, so it should become 1s
    MOVWF PR2
    
    ; end
    DELAY d'120', d'10'        ; around 40 ms
    BCF INTCON, INT0IF
    RETFIE


org 0x80
TIMER2_ISR:	; TIMER2 Interrupt Subroutine
    ; update LED
    MOVLW INCREMENT
    ADDWF counter, F
    
    ; end
    BCF PIR1, TMR2IF
    RETFIE


org 0x100
INIT:		; Initial Interrupt & I/O Port
    ; digital mode
    MOVLW 0x0f          ; Set ADCON1 register for digital mode
    MOVWF ADCON1        ; Store WREG value into ADCON1 register
    
    ; setup output port
    CLRF LATD
    ; Set RD4~7 as output (TRISD = 0000 1111)
    BCF TRISD, 4
    BCF TRISD, 5
    BCF TRISD, 6
    BCF TRISD, 7
    
    ; setup interrupt
    BSF RCON, IPEN		; Enable priority levels on interrupts
    BSF INTCON, GIEH		; Enables all high-priority interrupts (because IPEN=1 and GIEH=1)
    BSF INTCON, GIEL		; Enables all unmasked peripheral interrupts (because IPEN=1 and GIEL=1)
    ; TMR2 to PR2 Match Interrupt
    BCF PIR1, TMR2IF		; Clear TMR2 to PR2 Match Interrupt flag bit (if interrupt occurs, TMR2IF will become 1)
    BSF PIE1, TMR2IE		; Enables the TMR2 to PR2 match interrupt 
    BCF IPR1, TMR2IP		; Low priority (timer interrupt is high priority)
    ; INT0 Interrupt
    BSF INTCON, INT0IE		; Enables the INT0 external interrupt (allow us to use "INT0/RB0 pin" to trigger interrupt)
    BCF INTCON2, INTEDG0	; Interrupt on falling edge
    
    ; setup timer
    MOVLW D'00100000'	; set Internal Oscillator Frequency as 250kHz
    MOVWF OSCCON
    MOVLW b'11111111'	; Prescale=1:16 & Postscale=1:16 => TIMER2 += 1 every (16 * 16 * 4) * 4us
    MOVWF T2CON
    MOVLW D'61'		; setup Timer2 Period Register as 61, so that when TIMER2 reaches 61, it will interrupt CPU
    MOVWF PR2		; i.e. every 61 * (16 * 16 * 4) * 4us ~= 0.25s


MAIN:
    BRA MAIN

end