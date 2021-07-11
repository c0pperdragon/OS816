;:ts=8
R0	equ	1
R1	equ	5
R2	equ	9
R3	equ	13
	code
	xdef	~~cmd_dir
	func
~~cmd_dir:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L2
	tcs
	phd
	tcd
argc_0	set	4
argv_0	set	6
	pea	#^L1
	pea	#<L1
	jsl	~~print
	lda	<L2+2
	sta	<L2+2+6
	lda	<L2+1
	sta	<L2+1+6
	pld
	tsc
	clc
	adc	#L2+6
	tcs
	rtl
L2	equ	0
L3	equ	1
	ends
	efunc
	kdata
L1:
	db	$64,$69,$72,$20,$63,$6F,$6D,$6D,$61,$6E,$64,$20,$6E,$6F,$74
	db	$20,$69,$6D,$70,$6C,$65,$6D,$65,$6E,$74,$65,$64,$20,$79,$65
	db	$74,$0A,$00
	ends
	code
	xdef	~~cmd_run
	func
~~cmd_run:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L6
	tcs
	phd
	tcd
argc_0	set	4
argv_0	set	6
p_1	set	0
buffer_1	set	4
f_1	set	204
ex_1	set	208
	lda	<L6+argc_0
	cmp	#<$2
	bcs	L10001
	pea	#^L5
	pea	#<L5
	jsl	~~print
L9:
	lda	<L6+2
	sta	<L6+2+6
	lda	<L6+1
	sta	<L6+1+6
	pld
	tsc
	clc
	adc	#L6+6
	tcs
	rtl
L10001:
	pea	#^L5+29
	pea	#<L5+29
	ldy	#$2
	lda	[<L6+argv_0],Y
	pha
	lda	[<L6+argv_0]
	pha
	jsl	~~fopen
	sta	<L7+f_1
	stx	<L7+f_1+2
	ora	<L7+f_1+2
	bne	L10002
	pea	#<$11
	pea	#^L5+31
	pea	#<L5+31
	pea	#8
	jsl	~~printf
	bra	L9
L10002:
	pea	#<$3e8
	jsl	~~malloc
	sta	<L7+ex_1
	stx	<L7+ex_1+2
	lda	<L7+f_1+2
	pha
	lda	<L7+f_1
	pha
	pea	#<$3e8
	pea	#<$1
	lda	<L7+ex_1+2
	pha
	lda	<L7+ex_1
	pha
	jsl	~~fread
	lda	<L7+f_1+2
	pha
	lda	<L7+f_1
	pha
	jsl	~~fclose
	lda	<L7+ex_1+2
	pha
	lda	<L7+ex_1
	pha
	jsl	~~free
	bra	L9
L6	equ	212
L7	equ	1
	ends
	efunc
	kdata
L5:
	db	$4E,$6F,$20,$70,$72,$6F,$67,$72,$61,$6D,$20,$67,$69,$76,$65
	db	$6E,$20,$74,$6F,$20,$65,$78,$65,$63,$75,$74,$65,$0A,$00,$72
	db	$00,$43,$61,$6E,$20,$6E,$6F,$74,$20,$6F,$70,$65,$6E,$20,$65
	db	$78,$65,$63,$75,$74,$61,$62,$6C,$65,$2E,$20,$65,$72,$72,$6E
	db	$6F,$3A,$25,$64,$0A,$00
	ends
	code
	xdef	~~cmd_tokens
	func
~~cmd_tokens:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L12
	tcs
	phd
	tcd
argc_0	set	4
argv_0	set	6
i_1	set	0
	stz	<L13+i_1
	bra	L10006
L10005:
	pea	#^L11
	pea	#<L11
	jsl	~~print
	lda	<L13+i_1
	asl	A
	asl	A
	clc
	adc	<L12+argv_0
	sta	<R1
	lda	<L12+argv_0+2
	sta	<R1+2
	ldy	#$2
	lda	[<R1],Y
	pha
	lda	[<R1]
	pha
	jsl	~~print
	pea	#^L11+2
	pea	#<L11+2
	jsl	~~print
	inc	<L13+i_1
L10006:
	lda	<L13+i_1
	cmp	<L12+argc_0
	bcc	L10005
	lda	<L12+2
	sta	<L12+2+6
	lda	<L12+1
	sta	<L12+1+6
	pld
	tsc
	clc
	adc	#L12+6
	tcs
	rtl
L12	equ	10
L13	equ	9
	ends
	efunc
	kdata
L11:
	db	$5B,$00,$5D,$0A,$00
	ends
	code
	xdef	~~cmd_help
	func
~~cmd_help:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L17
	tcs
	phd
	tcd
argc_0	set	4
argv_0	set	6
	pea	#^L16
	pea	#<L16
	jsl	~~print
	pea	#^L16+20
	pea	#<L16+20
	jsl	~~print
	pea	#^L16+52
	pea	#<L16+52
	jsl	~~print
	pea	#^L16+122
	pea	#<L16+122
	jsl	~~print
	pea	#^L16+179
	pea	#<L16+179
	jsl	~~print
	lda	<L17+2
	sta	<L17+2+6
	lda	<L17+1
	sta	<L17+1+6
	pld
	tsc
	clc
	adc	#L17+6
	tcs
	rtl
L17	equ	0
L18	equ	1
	ends
	efunc
	kdata
L16:
	db	$42,$75,$69,$6C,$74,$2D,$69,$6E,$20,$63,$6F,$6D,$6D,$61,$6E
	db	$64,$73,$3A,$0A,$00,$20,$20,$20,$20,$68,$65,$6C,$70,$20,$20
	db	$20,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$2E,$20,$74,$68,$69,$73
	db	$20,$69,$6E,$66,$6F,$0A,$00,$20,$20,$20,$20,$64,$69,$72,$20
	db	$3C,$70,$72,$65,$66,$69,$78,$3E,$20,$2E,$2E,$2E,$20,$73,$68
	db	$6F,$77,$20,$61,$6C,$6C,$20,$66,$69,$6C,$65,$73,$20,$77,$68
	db	$6F,$73,$65,$20,$6E,$61,$6D,$65,$20,$73,$74,$61,$72,$74,$73
	db	$20,$77,$69,$74,$68,$3C,$20,$3C,$70,$72,$65,$66,$69,$78,$3E
	db	$0A,$00,$20,$20,$20,$20,$72,$75,$6E,$20,$3C,$66,$69,$6C,$65
	db	$3E,$20,$2E,$2E,$2E,$2E,$2E,$20,$6C,$6F,$61,$64,$20,$61,$6E
	db	$20,$65,$78,$65,$63,$75,$74,$61,$62,$6C,$65,$20,$66,$69,$6C
	db	$65,$20,$61,$6E,$64,$20,$72,$75,$6E,$20,$69,$74,$0A,$00,$20
	db	$20,$20,$20,$74,$6F,$6B,$65,$6E,$73,$20,$2E,$2E,$2E,$2E,$2E
	db	$2E,$2E,$2E,$2E,$20,$74,$65,$73,$74,$20,$63,$6F,$6D,$6D,$61
	db	$6E,$64,$20,$74,$6F,$20,$73,$68,$6F,$77,$20,$74,$6F,$6B,$65
	db	$6E,$69,$7A,$69,$6E,$67,$20,$72,$65,$73,$75,$6C,$74,$0A,$00
	ends
	code
	xdef	~~main
	func
~~main:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L21
	tcs
	phd
	tcd
argc_0	set	4
argv_0	set	6
buffer_1	set	0
tokens_1	set	200
numtokens_1	set	240
	pea	#<$1
	pea	#<$0
	pea	#^L20
	pea	#<L20
	lda	#<~~_iob+20
	sta	<R0
	xref	_BEG_DATA
	lda	#_BEG_DATA>>16
	sta	<R0+2
	pha
	pei	<R0
	pea	#14
	jsl	~~fprintf
	lda	<L21+argc_0
	beq	L23
	pea	#<$52
	pea	#^L20+68
	pea	#<L20+68
	pea	#^L20+60
	pea	#<L20+60
	jsl	~~_assert
	ldx	<R0+2
	lda	<R0
	bra	L25
L23:
	lda	#$0
	tax
L25:
	sep	#$20
	longa	off
	lda	|~~_ctype+66
	and	#<$7
	rep	#$20
	longa	on
	bne	L26
	pea	#<$53
	pea	#^L20+132
	pea	#<L20+132
	pea	#^L20+119
	pea	#<L20+119
	jsl	~~_assert
	ldx	<R0+2
	lda	<R0
	bra	L28
L26:
	lda	#$0
	tax
L28:
	phy
	phy
	phy
	phy
	pea	#$4016
	pea	#$0000
	pea	#$0000
	pea	#$0000
	jsl	~~sin
	ply
	ply
	ply
	ply
	phy
	phy
	phy
	phy
	pea	#$4016
	pea	#$0000
	pea	#$0000
	pea	#$0000
	jsl	~~sin
	ply
	ply
	ply
	ply
	xref	~~~dcmp
	jsl	~~~dcmp
	beq	L29
	pea	#<$54
	pea	#^L20+204
	pea	#<L20+204
	pea	#^L20+183
	pea	#<L20+183
	jsl	~~_assert
	ldx	<R0+2
	lda	<R0
	bra	L10009
L29:
	lda	#$0
	tax
L10009:
	pea	#^L20+255
	pea	#<L20+255
	jsl	~~print
	pea	#<$c8
	pea	#0
	clc
	tdc
	adc	#<L22+buffer_1
	pha
	jsl	~~input
	pea	#<$a
	pea	#0
	clc
	tdc
	adc	#<L22+tokens_1
	pha
	pea	#0
	clc
	tdc
	adc	#<L22+buffer_1
	pha
	jsl	~~tokenize
	sta	<L22+numtokens_1
	lda	#$0
	cmp	<L22+numtokens_1
	bcs	L10009
	pea	#^L20+257
	pea	#<L20+257
	lda	<L22+tokens_1+2
	pha
	lda	<L22+tokens_1
	pha
	jsl	~~strcmp
	tax
	bne	L10011
	pea	#0
	clc
	tdc
	adc	#<L22+tokens_1
	pha
	lda	<L22+numtokens_1
	pha
	jsl	~~cmd_dir
	bra	L10009
L10011:
	pea	#^L20+261
	pea	#<L20+261
	lda	<L22+tokens_1+2
	pha
	lda	<L22+tokens_1
	pha
	jsl	~~strcmp
	tax
	bne	L10013
	pea	#0
	clc
	tdc
	adc	#<L22+tokens_1
	pha
	lda	<L22+numtokens_1
	pha
	jsl	~~cmd_run
	brl	L10009
L10013:
	pea	#^L20+265
	pea	#<L20+265
	lda	<L22+tokens_1+2
	pha
	lda	<L22+tokens_1
	pha
	jsl	~~strcmp
	tax
	bne	L10015
	pea	#0
	clc
	tdc
	adc	#<L22+tokens_1
	pha
	lda	<L22+numtokens_1
	pha
	jsl	~~cmd_tokens
	brl	L10009
L10015:
	pea	#0
	clc
	tdc
	adc	#<L22+tokens_1
	pha
	lda	<L22+numtokens_1
	pha
	jsl	~~cmd_help
	brl	L10009
L21	equ	246
L22	equ	5
	ends
	efunc
	kdata
L20:
	db	$4F,$53,$38,$31,$36,$20,$63,$6F,$6D,$6D,$61,$6E,$64,$20,$6C
	db	$69,$6E,$65,$20,$69,$6E,$74,$65,$72,$66,$61,$63,$65,$20,$25
	db	$64,$2E,$25,$64,$20,$28,$63,$29,$20,$32,$30,$32,$31,$20,$52
	db	$65,$69,$6E,$68,$61,$72,$64,$20,$47,$72,$61,$66,$6C,$0A,$00
	db	$61,$72,$67,$63,$3D,$3D,$30,$00,$43,$3A,$5C,$55,$73,$65,$72
	db	$73,$5C,$52,$65,$69,$6E,$68,$61,$72,$64,$5C,$44,$6F,$63,$75
	db	$6D,$65,$6E,$74,$73,$5C,$47,$69,$74,$48,$75,$62,$5C,$4F,$53
	db	$38,$31,$36,$5C,$63,$6C,$69,$5C,$63,$6C,$69,$2E,$63,$00,$69
	db	$73,$61,$6C,$6E,$75,$6D,$28,$27,$41,$27,$29,$00,$43,$3A,$5C
	db	$55,$73,$65,$72,$73,$5C,$52,$65,$69,$6E,$68,$61,$72,$64,$5C
	db	$44,$6F,$63,$75,$6D,$65,$6E,$74,$73,$5C,$47,$69,$74,$48,$75
	db	$62,$5C,$4F,$53,$38,$31,$36,$5C,$63,$6C,$69,$5C,$63,$6C,$69
	db	$2E,$63,$00,$73,$69,$6E,$28,$35,$2E,$35,$29,$20,$3D,$3D,$20
	db	$73,$69,$6E,$28,$35,$2E,$35,$29,$00,$43,$3A,$5C,$55,$73,$65
	db	$72,$73,$5C,$52,$65,$69,$6E,$68,$61,$72,$64,$5C,$44,$6F,$63
	db	$75,$6D,$65,$6E,$74,$73,$5C,$47,$69,$74,$48,$75,$62,$5C,$4F
	db	$53,$38,$31,$36,$5C,$63,$6C,$69,$5C,$63,$6C,$69,$2E,$63,$00
	db	$3E,$00,$64,$69,$72,$00,$72,$75,$6E,$00,$74,$6F,$6B,$65,$6E
	db	$73,$00
	ends
	xref	~~sin
	xref	~~_assert
	xref	~~printf
	xref	~~fread
	xref	~~fprintf
	xref	~~fopen
	xref	~~fclose
	xref	~~strcmp
	xref	~~malloc
	xref	~~free
	xref	~~tokenize
	xref	~~input
	xref	~~print
	xref	~~_ctype
	xref	~~_iob
