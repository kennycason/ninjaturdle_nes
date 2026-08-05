; kenes.s is .include'd from crt0.s, so all ZP symbols (TEMP, SPRID, PTR, SCRX, SCRY, OAM_BUF, sp) are already defined.

.export _buffer_stub
.export _oam_meta_spr_clip
.export _oam_meta_spr_flip_h
.export _oam_meta_spr_flip_h_boss

.segment "CODE"

_buffer_stub:
	rts


;----------------------------------------------------------------------
; void __fastcall__ oam_meta_spr_clip(unsigned char x, unsigned char y, const unsigned char *data);
; Like oam_meta_spr but skips tiles whose X wraps past the screen edge.
;----------------------------------------------------------------------
_oam_meta_spr_clip:
	sta <PTR
	stx <PTR+1

	ldy #1
	lda (sp),y      ; x parameter
	dey
	sta <SCRX
	lda (sp),y      ; y parameter
	sta <SCRY

	ldx SPRID

@cloop:
	lda (PTR),y     ; x offset
	cmp #$80
	beq @cdone
	bcs @cneg       ; negative offset — never wraps right
	clc
	adc <SCRX
	bcs @cskip      ; positive offset overflowed, skip tile
	sta OAM_BUF+3,x
	bcc @ctile      ; always taken

@cneg:
	clc
	adc <SCRX
	sta OAM_BUF+3,x

@ctile:
	iny
	lda (PTR),y     ; y offset
	iny
	clc
	adc <SCRY
	sta OAM_BUF+0,x
	lda (PTR),y     ; tile
	iny
	sta OAM_BUF+1,x
	lda (PTR),y     ; attribute
	iny
	sta OAM_BUF+2,x
	inx
	inx
	inx
	inx
	jmp @cloop

@cskip:
	iny
	iny
	iny
	iny
	jmp @cloop

@cdone:
	lda <sp
	adc #1          ; carry is set here, adds 2
	sta <sp
	bcc @cnocarry
	inc <sp+1
@cnocarry:
	stx SPRID
	rts


;----------------------------------------------------------------------
; void __fastcall__ oam_meta_spr_flip_h(unsigned char x, unsigned char y, const unsigned char *data);
; H-flips: x = (x+6) - x_offset, attr |= 0x40. For 16x16 meta-sprites.
;----------------------------------------------------------------------
_oam_meta_spr_flip_h:
	sta <PTR
	stx <PTR+1
	ldy #1
	lda (sp),y      ; x parameter
	clc
	adc #6
	sta <SCRX
	dey
	jmp flip_shared

;----------------------------------------------------------------------
; void __fastcall__ oam_meta_spr_flip_h_boss(unsigned char x, unsigned char y, const unsigned char *data);
; Same but sum=22 for 32x32 boss sprites.
;----------------------------------------------------------------------
_oam_meta_spr_flip_h_boss:
	sta <PTR
	stx <PTR+1
	ldy #1
	lda (sp),y      ; x parameter
	clc
	adc #22
	sta <SCRX
	dey

flip_shared:
	lda (sp),y      ; y parameter (Y=0)
	sta <SCRY

	ldx SPRID

@floop:
	lda (PTR),y     ; x offset from data
	cmp #$80
	beq @fdone

	eor #$FF
	sec
	adc <SCRX       ; SCRX - x_offset
	sta OAM_BUF+3,x

	iny
	lda (PTR),y     ; y offset
	clc
	adc <SCRY
	sta OAM_BUF+0,x

	iny
	lda (PTR),y     ; tile
	sta OAM_BUF+1,x

	iny
	lda (PTR),y     ; attribute
	ora #$40        ; OAM_FLIP_H
	sta OAM_BUF+2,x

	iny
	inx
	inx
	inx
	inx
	jmp @floop

@fdone:
	lda <sp
	adc #1          ; carry set, adds 2
	sta <sp
	bcc @fnocarry
	inc <sp+1
@fnocarry:
	stx SPRID
	rts
