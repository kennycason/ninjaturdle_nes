; iNES header

; iNES identifier
.byte "NES",$1a 

; Number of PRG-ROM blocks
.byte $03

; Number of CHR-ROM blocks
.byte $01

; ROM control bytes: Horizontal mirroring, no SRAM
; or trainer, Mapper #0
;.byte $00, $00
.byte %00010000, %00000000			;MMC1 with SRAM

; Filler
.byte $00,$00,$00,$00,$00,$00,$00,$00	
		
SCREEN	EQU $2000
PPU0	EQU $2000
PPU1	EQU $2001
PPU_STATUS	EQU $2002

	;Zero page variables
	ENUM $0000
tempvec	db 0,0
tmp0	db 0
tmp1	db 0
tmp2	db 0
tmp3	db 0
tmp4	db 0
tmp5	db 0
tmp6	db 0
tmp7	db 0
	ENDE

;--------------------------------------------------------------------------------
; BANK 00
;--------------------------------------------------------------------------------
	ORG $0000
	BASE $8000
bank00	inc $FE
	rts
	PAD $BFFA
	DW vblank, reset, irq
;--------------------------------------------------------------------------------
; BANK 01
;--------------------------------------------------------------------------------

	BASE $8000
bank01	inc $FF
	rts
	PAD $BFFA
	DW vblank, reset, irq

;--------------------------------------------------------------------------------
; FIXED BANK
;--------------------------------------------------------------------------------
		
	ORG $C000
reset	sei

	jsr vblankwait
	jsr resetMMC1
	lda #%01100	;Set bank layout
	jsr setMMC1r0
	lda #$00
	jsr setPRGBank
		
	;clear RAM
	lda #$00
	ldx #$00
_reset_0	sta $0000,x
	sta $0100,x
	sta $0200,x
	sta $0300,x
	sta $0400,x
	sta $0500,x
	sta $0600,x
	sta $0700,x
	inx
	bne _reset_0

	;reset Stack
	ldx #$FF
	txs

	lda #$00
	sta PPU0
	sta PPU1

	;set PPU registers
	lda #%10001000
	sta PPU0
	lda #%00011010
	sta PPU1

main_loop	jmp main_loop
	
vblankwait	bit $2002
	bpl vblankwait
	rts
	
vblankendwait	bit $2002
	bmi vblankendwait
	rts

vblank	;rti
	pha
	txa
	pha
	tya
	pha
	bit $2002

	jsr screen_off

	lda #>SCREEN
	sta $2006
	lda #<SCREEN
	sta $2006
	lda $Fe		;write content of $FE and $FE to screen
	sta $2007
	lda $FF
	sta $2007
	
	lda #%00011110
	jsr screen_on
	
	lda #$00		;call routine in bank 00 to inc $FE
	jsr setPRGBank
	jsr bank00

	lda #$01		;call routine in bank 01 to inc $FF
	jsr setPRGBank
	jsr bank01
	
	pla
	tay
	pla
	tax
	pla
	rti

irq	rti

setPRGBank	
	sta $E000
	lsr a
	sta $E000
	lsr a
	sta $E000
	lsr a
	sta $E000
	lsr a
	sta $E000
	rts
	
screen_on	sta $2001
	ldx #$00
	stx $2005
	stx $2005
	stx $2006
	stx $2006
	rts
	
screen_off	lda #%00010000
	sta $2001
	sta $2005
	sta $2005
	sta $2006
	sta $2006
	rts
			
resetMMC1	ldx #$7F
@b	inx
	bne @a
	rts
@a	stx $8000
	bne @b
	rts
	
setMMC1r0	sta $9fff
	lsr a
	sta $9fff
	lsr a
	sta $9fff
	lsr a
	sta $9fff
	lsr a
	sta $9fff
	rts


	ORG $FFFA
	DW vblank, reset, irq
	
;--------------------------------------------------------------------------------
; CHR ROM
;--------------------------------------------------------------------------------
	BASE $0000
	incbin "set2.chr"
	;incbin "set2.chr"
	ALIGN $2000
	
;--------------------------------------------------------------------------------
; END OF ROM
;--------------------------------------------------------------------------------


	