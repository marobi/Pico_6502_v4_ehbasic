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
VDU_FP  = $D029  ; Flip Sprite drawing :: 0 - NONE, 1 - HORZ, 2 - VERT, currently not supported
VDU_SID = $D02A  ; Sprite Id
VDU_DMOD= $D02B	 ; draw mode :: 0 - NOP, 1 - OR, 2 - AND, 3 - XOR, currently allway XOR
VDU_MOD = $D02C  ; VDU mode
VDU_PAL = $D02D  ; PAL index
VDU_COL = $D02E  ; FG color low
VDU_BCOL= $D02F  ; FG color high/BG color

VDU_RET = $D030  ; return value
 
VDU_BUFFER = $D100; 256 bytes buffer for sprite definitions
VDU_BUFEND = VDU_BUFFER+256

;--------------------------------
; VDU engine
CMD_CLS    = $01
CMD_MOVE   = $02
CMD_COLOR  = $03
CMD_PIXEL  = $04
CMD_DRAW   = $05
CMD_RECT   = $06
CMD_CIRCLE = $07
CMD_PAL    = $08
CMD_VDU    = $09
CMD_REFRESH= $0A
CMD_TRI    = $0B

; SPRITE engine
CMD_SPRITE = $10
CMD_CSPRITE= $11
CMD_SDRAW  = $12
CMD_SCLEAR = $13
CMD_SMOVE  = $14
CMD_COLL   = $15

CMD_SANE   = $FF

;--------------------------------
; SOUND
SND_CMD = $D040	 ; CMD register
SND_NOTE= $D041	 ; NOTE
SND_DUR = $D042	 ; DURATION

;--------------------------------
VDU_WAIT:			; wait for cmd register to empty
	LDA VDU_CMD
	BNE VDU_WAIT	; not empty
	RTS

VDU_GETARG:
	JSR	LAB_EVNM	; evaluate expression and check is numeric,
					; else do type mismatch
	JSR	LAB_F2FX	; convert floating-to-fixed
	LDA Itempl
	RTS

VDU_NEXTARG:
	JSR	LAB_1C01	; scan for ','
	JMP	VDU_GETARG
	
VDU_GETX:
	JSR	VDU_GETARG
	STA VDU_XL
	LDA Itemph
	STA VDU_XH
	RTS
	
VDU_GETY:
	JSR	VDU_NEXTARG
	STA VDU_YL
	LDA Itemph
	STA VDU_YH
	RTS

VDU_GETW:
	JSR VDU_NEXTARG
	STA VDU_WL
	LDA Itemph
	STA VDU_WH
	RTS

VDU_GETH:
	JSR VDU_NEXTARG
	STA VDU_HL
	LDA Itemph
	STA VDU_HH
	RTS
	
;--------------------------------------------------------------------
LAB_SANE:
	JSR VDU_WAIT
	LDA	#CMD_SANE		; SANE
	STA VDU_CMD
	RTS
	
LAB_CLS:
	JSR VDU_WAIT
	LDA #CMD_CLS	; CLS
	STA VDU_CMD
	RTS

LAB_VDU:
	JSR VDU_WAIT
	JSR	VDU_GETARG
	STA VDU_MOD		; vdu mode
	LDA #CMD_VDU	; VDU
	STA VDU_CMD
	RTS
	
LAB_MOVE:
	JSR VDU_WAIT	; wait for previous command is finished
	JSR VDU_GETX
	JSR VDU_GETY
	LDA #CMD_MOVE	; MOVE
	STA VDU_CMD
	RTS
	
LAB_DRAW:
	JSR VDU_WAIT	; wait for previous command is finished
	JSR VDU_GETX
	JSR VDU_GETY
					; NEED to add MODE
	LDA #CMD_DRAW	; DRAW
	STA VDU_CMD
	RTS

LAB_PIXEL:
	JSR VDU_WAIT
	JSR VDU_GETX
	JSR VDU_GETY
					; NEED to add MODE
	LDA #CMD_PIXEL	; PIXEL
	STA VDU_CMD
	RTS

LAB_RECT:
	JSR VDU_WAIT
	JSR VDU_GETX
	JSR VDU_GETY
	JSR VDU_GETW
	JSR VDU_GETH
	JSR	VDU_NEXTARG
	STA VDU_DMOD
	LDA #CMD_RECT	; RECT
	STA VDU_CMD
	RTS
	
LAB_CIRCLE:
	JSR VDU_WAIT
	JSR VDU_GETX
	JSR VDU_GETY
	JSR VDU_GETW	; radius
	JSR	VDU_NEXTARG
	STA VDU_DMOD
	LDA #CMD_CIRCLE	; CIRCLE
	STA VDU_CMD
	RTS

LAB_TRIANGLE:
	JSR VDU_WAIT
	JSR VDU_GETX	; 2nd pt X
	JSR VDU_GETY
	JSR VDU_GETW	; 3rd pt X
	JSR VDU_GETH	; 3rd pt Y
	JSR	VDU_NEXTARG
	STA VDU_DMOD	
	LDA #CMD_TRI	; Triangle
	STA VDU_CMD
	RTS

LAB_COLOR:
	JSR VDU_WAIT
	JSR	VDU_GETARG
	STA VDU_COL
	LDA #CMD_COLOR	; COLOR
	STA VDU_CMD
	RTS
	
LAB_PALETTE:
	JSR	VDU_GETARG
	STA VDU_PAL
	JSR VDU_NEXTARG
	STA VDU_COL
	LDA Itemph
	STA VDU_BCOL
	LDA #CMD_PAL	; PALETTE
	STA VDU_CMD
	RTS

LAB_VREFRESH:
	JSR VDU_WAIT
	LDA #CMD_REFRESH ; REFRESH
	STA VDU_CMD
	RTS

;---------------------------------------------------------
LAB_SDIM:			; BYTE
    JSR VDU_WAIT
	JSR	VDU_GETARG
;	LDA Itempl		; Index

SDIM_NEXT:
	PHA				; Save index
	JSR	LAB_GBYT	; check next BASIC byte
	CMP	#','		; is there more ?
	BNE SDIM_DONE
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	PLA
	TAY				; Index
	LDA Itempl
	STA VDU_BUFFER, Y
	INY
	TYA
    BNE SDIM_NEXT
	LDX	#$0A		; overflow
	JMP LAB_XERR	; WARM start
	
SDIM_DONE:
	PLA				; dump index
	RTS

;----------
LAB_WDIM:
    JSR VDU_WAIT
	JSR	VDU_GETARG
	LDA Itempl		; Index

WDIM_NEXT:
	PHA				; Save index
	JSR	LAB_GBYT	; check next BASIC byte
	CMP	#','		; is there more ?
	BNE WDIM_DONE
	JSR	LAB_1C01	; scan for ','
	JSR	VDU_GETARG
	PLA
	TAY				; Index
	LDA Itempl
	STA VDU_BUFFER, Y
	INY
	LDA Itemph
	STA VDU_BUFFER, Y
	INY
	TYA
    BNE WDIM_NEXT
	LDX	#$0A		; overflow
	JMP LAB_XERR	; WARM start
	
WDIM_DONE:			; WORD
	PLA				; dump index
	RTS

;------------
LAB_SPRITE:			; Id W H M F O
	JSR	VDU_WAIT
	JSR VDU_GETARG
	STA VDU_SID		; id
	JSR VDU_GETW	; width
	JSR VDU_GETH	; height
	JSR	VDU_NEXTARG
	STA VDU_DMOD	; mode
	JSR	VDU_NEXTARG
	STA VDU_FP		; flip
	JSR	VDU_NEXTARG
	STA VDU_PAL		; offset
	LDA #CMD_SPRITE
	STA	VDU_CMD
	RTS

;------------
LAB_CSPRITE:
	RTS

;------------
LAB_SDRAW:	; Id X Y C
	JSR VDU_WAIT
	JSR VDU_GETARG
	STA VDU_SID		; id
	JSR	LAB_1C01	; scan for ','
	JSR VDU_GETX	; x
	JSR VDU_GETY	; y
	JSR	VDU_NEXTARG
	STA	VDU_COL
	LDA #CMD_SDRAW
	STA	VDU_CMD
	RTS

;------------
LAB_SMOVE:			; Id (dx dy (mod))
	JSR VDU_WAIT
	JSR VDU_GETARG
	STA VDU_SID		; id
	JSR	LAB_GBYT	; check next BASIC byte
	CMP	#','		; is there more ?
	BNE SMOVE_DONE  ; nope
	JSR	LAB_1C01	; scan for ','
	JSR VDU_GETX
	JSR VDU_GETY
	JSR	LAB_GBYT	; check next BASIC byte
	CMP	#','		; is there more ?
	BNE SMOVE_CONT  ; nope
	JSR VDU_NEXTARG
	STA	VDU_MOD		; modifier
    JMP SMOVE_CMD
SMOVE_CONT:
	LDA #$00
	STA VDU_MOD
SMOVE_CMD:
	LDA #CMD_SMOVE
	STA	VDU_CMD
	RTS
	
SMOVE_DONE:
	LDA #$00
	STA VDU_XL
	STA VDU_XH
	STA VDU_YL
	STA VDU_YH
	STA VDU_MOD
	BEQ SMOVE_CMD	; always
	
;-----------
LAB_SCLEAR:
	RTS
	
;------------
LAB_COLLISION:
	JSR VDU_WAIT
	JSR	LAB_F2FX	; convert floating-to-fixed
	LDA Itempl
	STA VDU_SID
	LDA #CMD_COLL
	STA VDU_CMD
COLL_WAIT:
	LDA VDU_RET
	BEQ COLL_WAIT
	AND #$7F
	TAY
	LDA #$00
	STA VDU_RET
	JMP	LAB_1FD0	; convert Y to byte in FAC1 and return
	
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
	JSR	VDU_NEXTARG
	STA SND_DUR
	LDA #$01
	STA SND_CMD
	RTS
