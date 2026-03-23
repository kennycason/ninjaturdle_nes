; kenes.s is .include'd from crt0.s, so all ZP symbols (TEMP, SPRID, PTR, SCRX, SCRY, OAM_BUF, sp) are already defined.

.export _buffer_stub
.export _oam_meta_spr_flip_h
.export _oam_meta_spr_flip_h_boss

.segment "CODE"

_buffer_stub:
	rts  ; Do nothing and return immediately


;----------------------------------------------------------------------
; void __fastcall__ oam_meta_spr_flip_h(unsigned char x, unsigned char y, const unsigned char *data);
; Identical to oam_meta_spr but H-flips: x = (x+6) - x_offset, attr |= 0x40
; For 16x16 meta-sprites (sum of min_x + max_x = 6)
;----------------------------------------------------------------------
_oam_meta_spr_flip_h:
	sta <PTR
	stx <PTR+1

	ldy #1
	lda (sp),y      ; x parameter
	dey
	clc
	adc #6          ; base_x = x + 6
	sta <SCRX
	lda (sp),y      ; y parameter
	sta <SCRY

	ldx SPRID

@loop:
	lda (PTR),y     ; x offset from data
	cmp #$80
	beq @done

	; X flip: base_x - x_offset
	eor #$FF        ; ~x_offset
	sec             ; +1 for two's complement
	adc <SCRX       ; SCRX + (~x_offset + 1) = SCRX - x_offset
	sta OAM_BUF+3,x

	iny
	lda (PTR),y     ; y offset
	clc
	adc <SCRY
	sta OAM_BUF+0,x

	iny
	lda (PTR),y     ; tile index
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
	jmp @loop

@done:
	lda <sp
	adc #1          ; carry is set here, adds 2
	sta <sp
	bcc @nocarry
	inc <sp+1
@nocarry:
	stx SPRID
	rts


;----------------------------------------------------------------------
; void __fastcall__ oam_meta_spr_flip_h_boss(unsigned char x, unsigned char y, const unsigned char *data);
; Same as above but sum=22 for 32x32 boss sprites
;----------------------------------------------------------------------
_oam_meta_spr_flip_h_boss:
	sta <PTR
	stx <PTR+1

	ldy #1
	lda (sp),y      ; x parameter
	dey
	clc
	adc #22         ; base_x = x + 22 (boss sum)
	sta <SCRX
	lda (sp),y      ; y parameter
	sta <SCRY

	ldx SPRID

@loop2:
	lda (PTR),y     ; x offset from data
	cmp #$80
	beq @done2

	eor #$FF
	sec
	adc <SCRX
	sta OAM_BUF+3,x

	iny
	lda (PTR),y     ; y offset
	clc
	adc <SCRY
	sta OAM_BUF+0,x

	iny
	lda (PTR),y     ; tile index
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
	jmp @loop2

@done2:
	lda <sp
	adc #1
	sta <sp
	bcc @nocarry2
	inc <sp+1
@nocarry2:
	stx SPRID
	rts
