; > monmin.asm
; minimal monitor for EhBASIC 2.22 running under the 2m5.de 6502 emulator
; 25-mar-2014 - save/load to match V0.83 or higher DMA register

; some minor modifications are in the basic program itself

; RAM scan has been removed.
; maximum RAM can be defined here, you can still configure less RAM during at coldstart

; 32k RAM mirrored = 12k EhBASIC code at top + 20k work space at bottom

	.org	$A000

	JMP	RES_vec		;startup soft reset

	.INCLUDE "basic.asm"

; Defines
KBD         = $D010          ; input register
DSP         = $D012          ; output register

; IO setup for the 2m5.de 6502 emulator environment
IO_PAGE	    = $BFF0			; set I/O area +$f0 (internal registers)

EMU_flag	= IO_PAGE+$f	; bit 0 = 10ms tick
EMU_diag	= IO_PAGE+$c	; bit 7 = force debugger
IR_mask		= IO_PAGE+$e	; interrupt enable mask

; now the code. all this does is set up the vectors and interrupt code
; and wait for the user to select [C]old or [W]arm start. nothing else
; fits in less than 256 bytes

;	.org	$FE00		; pretend this is in a 1/2K ROM

; reset vector points here
RES_vec
	CLD				; clear decimal mode
	CLI
	LDX	#$FF		; empty stack
	TXS				; set the stack
	
; set up vectors and interrupt code, copy them to page 2
	LDY	#END_vec-LAB_vec; set index/count
LAB_stlp
	LDA	LAB_vec-1,Y	; get byte from interrupt code
	STA	VEC_IN-1,Y	; save to RAM
	DEY			; decrement index/count
	BNE	LAB_stlp	; loop if more to do

; now do the sign-on message, Y = $00 here
LAB_signon
	LDA	LAB_mess,Y	; get byte from sign on message
	BEQ	LAB_nokey	; exit loop if done

	JSR	V_OUTP		; output character
	INY				; increment index
	BNE	LAB_signon	; loop, branch always

LAB_nokey
	JSR	V_INPT		; call scan input device
	BCC	LAB_nokey	; loop if no key

	AND	#$DF		; mask xx0x xxxx, ensure upper case
	CMP	#'W'		; compare with [W]arm start
	BEQ	LAB_dowarm	; branch if [W]arm start

	CMP	#'C'		; compare with [C]old start
	BNE	RES_vec		; loop if not [C]old start

	JMP	LAB_COLD	; do EhBASIC cold start

LAB_dowarm
	JMP	LAB_WARM	; do EhBASIC warm start

; Byte out to serial console
SCRNout
    PHA             ;*Save A
WAIT:
	LDA DSP
	BNE WAIT
	PLA
	STA DSP
	RTS
     
; byte in from emulated ACIA
KBDin
	LDA KBD
	BEQ LAB_nobyw
	PHA			; save key
	LDA #$00	; clear register
	STA KBD
	PLA			; restore key
	CMP 	#127    ; convert delete to backspace
	BNE 	conv_bs2del
	LDA 	#8
conv_bs2del
	CMP 	#27     ; escape?
	BNE 	skip_esc_no
	TXA       		; discard escape sequence
	PHA
	LDX 	#5		; timer loop - 5*10ms
skip_esc_next
	LDA 	#1		; ack last tick
	STA 	EMU_flag
skip_esc_wait  
	LDA 	EMU_flag
	AND 	#1		; next tick
	BEQ 	skip_esc_wait
	DEX
	BNE 	skip_esc_next
skip_esc_discard  
	INX			; any data = X > 1
	LDA 	KBD
	BEQ 	skip_esc_discard
	CPX 	#1
	BEQ 	skip_esc_esc
	PLA			; was special key - skip
	TAX
	LDA 	#0
	CLC
	RTS
skip_esc_esc			; escape only - send to basic  
	PLA
	TAX
	LDA 	#27

skip_esc_no
	EOR 	#0		; set flags NZ
	SEC			; flag byte received
	RTS

LAB_nobyw
	CLC			; flag no byte received
	RTS

; LOAD - currently does nothing.
SBCload				        ; load vector for EhBASIC
	RTS

; SAVE - currently does nothing.
SBCsave				        ; save vector for EhBASIC
	RTS

MON_INIT
	RTS

; vector tables
LAB_vec
	.word	KBDin		; byte in from simulated ACIA
	.word	SCRNout		; byte out to simulated ACIA
	.word	SBCload		; load program 
	.word	SBCsave		; save program 
	.word	IRQ_CODE	; vector to handle IRQ 
	.word	NMI_CODE	; vector to handle IRQ 
END_vec


; EhBASIC IRQ support
IRQ_vec
;	JMP	(IRQ_indirect)	; jump through a vector in RAM
				; uncomment if needed
IRQ_CODE
	STA	irq_a_reg	; save A
	LDA	IrqBase		; get the IRQ flag byte
	BNE	IRQ_IS_SET	; Basic ready to handle?
	STA	IR_mask		; no, silence internal interrupts 
IRQ_IS_SET
	ORA	#$20		; 00100000 set IRQ pending
	STA	IrqBase		; save the new IRQ flag byte
	PLA			; pop saved flags
	ORA	#4		; set interrupt disable on stack
	PHA			; save flags again for RTI
	LDA	irq_a_reg	; restore A
	RTI			; return disabled!
	
; EhBASIC NMI support
NMI_vec
;	JMP	(NMI_indirect)	; jump through a vector in RAM
				; uncomment if needed
NMI_CODE
	PHA			; save A
	LDA	NmiBase		; get the NMI flag byte
	ORA	#$20		; 00100000 set NMI pending
	STA	NmiBase		; save the new NMI flag byte
	PLA			; restore A
	RTI

LAB_mess
	.byte	$0D,$0A,"NEO6502 EhBASIC start [C]old/[W]arm ?",$00
				; sign on string

	.INCLUDE	"neo6502.asm"

; system vectors
	.word	NMI_vec		; NMI vector
	.word	RES_vec		; RESET vector
	.word	IRQ_vec		; IRQ vector
