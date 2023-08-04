;
;
;
VDU_CMD = $D020  ; CMD register
VDU_XL  = $D021  ; X low
VDU_XH  = $D022  ; X high
VDU_YL  = $D023  ; Y low
VDU_YH  = $D024  ; Y high
VDU_WL  = $D025  ; Width low
VDU_WH  = $D026  ; Width high
VDU_HL  = $D027  ; Height low
VDU_HH  = $D028  ; Height high

VDU_DMOD= $D02B	 ; draw mode
VDU_MOD = $D02C  ; VDU mode
VDU_PAL = $D02D  ; PAL index
VDU_BCOL= $D02E  ; BG color -- TBI
VDU_COL = $D02F  ; FG color

;--------------------------------
SND_CMD = $D030	 ; CMD register
SND_NOTE= $D031	 ; NOTE
SND_DUR = $D032	 ; DURATION

;--------------------------------
VDU_WAIT:			; wait for cmd register to empty
	LDA VDU_CMD
	BNE VDU_WAIT	; not empty
	RTS

VDU_GETARG:
	JSR	LAB_EVNM	; evaluate expression and check is numeric,
					; else do type mismatch
	JMP	LAB_F2FX	; convert floating-to-fixed

VDU_GETX:
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_XL
	LDA Itemph
	STA VDU_XH
	RTS
	
VDU_GETY:
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_YL
	LDA Itemph
	STA VDU_YH
	RTS

VDU_GETW:
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_WL
	LDA Itemph
	STA VDU_WH
	RTS

VDU_GETH:
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_HL
	LDA Itemph
	STA VDU_HH
	RTS
	
;--------------------------------------------------------------------
LAB_SANE:
	JSR VDU_WAIT
	LDA	#$FF		; SANE
	STA VDU_CMD
	RTS
	
LAB_CLS:
	JSR VDU_WAIT
	LDA #$01		; CLS
	STA VDU_CMD
	RTS

LAB_VDU:
	JSR VDU_WAIT
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_MOD		; vdu mode
	LDA #$09		; VDU
	STA VDU_CMD
	RTS
	
LAB_MOVE:
	JSR VDU_WAIT	; wait for previous command is finished
	JSR VDU_GETX
	JSR VDU_GETY
	LDA #$02		; MOVE
	STA VDU_CMD
	RTS
	
LAB_DRAW:
	JSR VDU_WAIT	; wait for previous command is finished
	JSR VDU_GETX
	JSR VDU_GETY
					; NEED to add MODE
	LDA #$05		; DRAW
	STA VDU_CMD
	RTS

LAB_PIXEL:
	JSR VDU_WAIT
	JSR VDU_GETX
	JSR VDU_GETY
					; NEED to add MODE
	LDA #$04		; PIXEL
	STA VDU_CMD
	RTS

LAB_RECT:
	JSR VDU_WAIT
	JSR VDU_GETX
	JSR VDU_GETY
	JSR VDU_GETW
	JSR VDU_GETH
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_DMOD
	LDA #$06		; RECT
	STA VDU_CMD
	RTS
	
LAB_CIRCLE:
	JSR VDU_WAIT
	JSR VDU_GETX
	JSR VDU_GETY
	JSR VDU_GETW	; radius
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_DMOD
	LDA #$07		; CIRCLE
	STA VDU_CMD
	RTS

LAB_TRIANGLE:
	JSR VDU_WAIT
	JSR VDU_GETX	; 2nd pt X
	JSR VDU_GETY
	JSR VDU_GETW	; 3rd pt X
	JSR VDU_GETH	; 3rd pt Y
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_DMOD	
	LDA #$0B
	STA VDU_CMD
	RTS

LAB_COLOR:
	JSR VDU_WAIT
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_COL
	LDA #$03		; COLOR
	STA VDU_CMD
	RTS
	
LAB_PALETTE:
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_PAL
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	LDA Itempl
	STA VDU_COL
	LDA #$08		; PALETTE
	STA VDU_CMD
	RTS

LAB_VREFRESH:
	JSR VDU_WAIT
	LDA #$0A		; REFRESH
	STA VDU_CMD
	RTS

;---------------------------------------------------------
SND_WAIT:
	LDA SND_CMD
	BNE SND_WAIT
	RTS
	
LAB_SILENCE:
	JSR SND_WAIT
	LDA #$02
	STA SND_CMD
	RTS
	
LAB_PLAY:
	JSR SND_WAIT
	JSR VDU_GETARG
	LDA Itempl
	STA SND_NOTE
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	LDA Itempl
	STA SND_DUR
	LDA #$01
	STA SND_CMD
	RTS
