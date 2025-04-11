.import popax

.export _buffer_5_mt

.segment "CODE"

_buffer_5_mt:
	; a = index into DATA_PTR
	and #$ee        ; align index (even rows only)
	tay
	lda (DATA_PTR), y
	sta TEMP+2
	iny
	lda (DATA_PTR), y
	sta TEMP+3
	tya
	clc
	adc #15
	cmp #$f0        ; check bounds
	bcs @skip
	tay
	lda (DATA_PTR), y
	sta TEMP+4
	iny
	lda (DATA_PTR), y
	sta TEMP+5
@skip:

	; 4 metatile values now in TEMP+2 to TEMP+5

	jsr popax       ; get PPU address
	and #$9c
	sta TEMP+7      ; original low address
	stx TEMP+8

	sta TEMP
	txa
	ora #$40
	sta TEMP+1      ; prep high byte for VRAM update

	lda #0
	sta TEMP+6      ; loop counter (0 to 3)
	ldx VRAM_INDEX

@loop:
	; write first half (left metatile)
	lda TEMP
	sta VRAM_BUF+1,x
	lda TEMP+1
	sta VRAM_BUF,x
	jsr @inc_addr

	lda #4
	sta VRAM_BUF+2,x
	sta VRAM_BUF+9,x

	jsr @load_tile_index
	lda (META_PTR), y
	sta VRAM_BUF+3,x
	iny
	lda (META_PTR), y
	sta VRAM_BUF+4,x
	iny
	lda (META_PTR), y
	sta VRAM_BUF+10,x
	iny
	lda (META_PTR), y
	sta VRAM_BUF+11,x
	jsr @shift_palette_bits

	; write right half (next metatile)
	lda TEMP
	sta VRAM_BUF+8,x
	lda TEMP+1
	sta VRAM_BUF+7,x
	jsr @inc_addr

	inc TEMP+6
	jsr @load_tile_index
	lda (META_PTR), y
	sta VRAM_BUF+5,x
	iny
	lda (META_PTR), y
	sta VRAM_BUF+6,x
	iny
	lda (META_PTR), y
	sta VRAM_BUF+12,x
	iny
	lda (META_PTR), y
	sta VRAM_BUF+13,x
	jsr @shift_palette_bits

	txa
	clc
	adc #14
	tax

	jsr @check_wrap
	bne @done

	inc TEMP+6
	ldy TEMP+6
	cpy #4
	bcc @loop

@done:
	; write attribute byte (TEMP+10 = palette bits)
	lsr TEMP+8
	ror TEMP+7
	lsr TEMP+8
	ror TEMP+7
	lda TEMP+7
	pha
	and #7
	sta TEMP
	pla
	lsr a
	lsr a
	and #$f8
	ora #$c0
	ora TEMP
	sta TEMP
	lda TEMP+8
	asl a
	asl a
	ora #$23
	sta TEMP+1

	lda TEMP+1
	sta VRAM_BUF,x
	inx
	lda TEMP
	sta VRAM_BUF,x
	inx
	lda TEMP+10
	sta VRAM_BUF,x
	inx
	lda #$ff
	sta VRAM_BUF,x
	stx VRAM_INDEX
	rts

@inc_addr:
	tay
	lda TEMP
	clc
	adc #$20
	sta TEMP
	bcc @done_addr
	iny
@done_addr:
	sty TEMP+1
	rts

@load_tile_index:
	ldy TEMP+6
	lda TEMP+2,y
	sta TEMP+9
	asl a
	asl a
	clc
	adc TEMP+9
	clc
	adc TEMP+9
	tay
	rts

@check_wrap:
	ldy #0
	lda TEMP+8
	and #$03
	cmp #$03
	bne @not_wrap
	lda TEMP+7
	cmp #$80
	bcc @not_wrap
	iny
	lsr TEMP+10
	lsr TEMP+10
	lsr TEMP+10
	lsr TEMP+10
@not_wrap:
	tya
	rts

@shift_palette_bits:
	iny
	lda (META_PTR), y
	and #3
	ror a
	ror TEMP+10
	ror a
	ror TEMP+10
	rts