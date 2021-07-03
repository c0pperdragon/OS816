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
	pea	#<$0
	lda	#$1
	tax
	lda	#$8
	xref	~~~lcal
	jsl	~~~lcal
	bra	L9
L6	equ	0
L7	equ	1
	ends
	efunc
	kdata
L5:
	db	$4E,$6F,$20,$70,$72,$6F,$67,$72,$61,$6D,$20,$67,$69,$76,$65
	db	$6E,$20,$74,$6F,$20,$65,$78,$65,$63,$75,$74,$65,$0A,$00
	ends
	code
	xdef	~~cmd_tokens
	func
~~cmd_tokens:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L11
	tcs
	phd
	tcd
argc_0	set	4
argv_0	set	6
i_1	set	0
	stz	<L12+i_1
	bra	L10005
L10004:
	pea	#^L10
	pea	#<L10
	jsl	~~print
	lda	<L12+i_1
	asl	A
	asl	A
	clc
	adc	<L11+argv_0
	sta	<R1
	lda	<L11+argv_0+2
	sta	<R1+2
	ldy	#$2
	lda	[<R1],Y
	pha
	lda	[<R1]
	pha
	jsl	~~print
	pea	#^L10+2
	pea	#<L10+2
	jsl	~~print
	inc	<L12+i_1
L10005:
	lda	<L12+i_1
	cmp	<L11+argc_0
	bcc	L10004
	lda	<L11+2
	sta	<L11+2+6
	lda	<L11+1
	sta	<L11+1+6
	pld
	tsc
	clc
	adc	#L11+6
	tcs
	rtl
L11	equ	10
L12	equ	9
	ends
	efunc
	kdata
L10:
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
	sbc	#L16
	tcs
	phd
	tcd
argc_0	set	4
argv_0	set	6
	pea	#^L15
	pea	#<L15
	jsl	~~print
	pea	#^L15+20
	pea	#<L15+20
	jsl	~~print
	pea	#^L15+52
	pea	#<L15+52
	jsl	~~print
	pea	#^L15+122
	pea	#<L15+122
	jsl	~~print
	pea	#^L15+179
	pea	#<L15+179
	jsl	~~print
	lda	<L16+2
	sta	<L16+2+6
	lda	<L16+1
	sta	<L16+1+6
	pld
	tsc
	clc
	adc	#L16+6
	tcs
	rtl
L16	equ	0
L17	equ	1
	ends
	efunc
	kdata
L15:
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
	sbc	#L20
	tcs
	phd
	tcd
buffer_1	set	0
tokens_1	set	200
numtokens_1	set	240
	pea	#^L19
	pea	#<L19
	jsl	~~print
L10008:
	pea	#^L19+59
	pea	#<L19+59
	jsl	~~print
	pea	#<$c8
	pea	#0
	clc
	tdc
	adc	#<L21+buffer_1
	pha
	jsl	~~readline
	pea	#<$a
	pea	#0
	clc
	tdc
	adc	#<L21+tokens_1
	pha
	pea	#0
	clc
	tdc
	adc	#<L21+buffer_1
	pha
	jsl	~~tokenize
	sta	<L21+numtokens_1
	lda	#$0
	cmp	<L21+numtokens_1
	bcs	L10008
	pea	#^L19+61
	pea	#<L19+61
	lda	<L21+tokens_1+2
	pha
	lda	<L21+tokens_1
	pha
	jsl	~~strequal
	tax
	beq	L10010
	pea	#0
	clc
	tdc
	adc	#<L21+tokens_1
	pha
	lda	<L21+numtokens_1
	pha
	jsl	~~cmd_dir
	bra	L10008
L10010:
	pea	#^L19+65
	pea	#<L19+65
	lda	<L21+tokens_1+2
	pha
	lda	<L21+tokens_1
	pha
	jsl	~~strequal
	tax
	beq	L10012
	pea	#0
	clc
	tdc
	adc	#<L21+tokens_1
	pha
	lda	<L21+numtokens_1
	pha
	jsl	~~cmd_run
	brl	L10008
L10012:
	pea	#^L19+69
	pea	#<L19+69
	lda	<L21+tokens_1+2
	pha
	lda	<L21+tokens_1
	pha
	jsl	~~strequal
	tax
	beq	L10014
	pea	#0
	clc
	tdc
	adc	#<L21+tokens_1
	pha
	lda	<L21+numtokens_1
	pha
	jsl	~~cmd_tokens
	brl	L10008
L10014:
	pea	#0
	clc
	tdc
	adc	#<L21+tokens_1
	pha
	lda	<L21+numtokens_1
	pha
	jsl	~~cmd_help
	brl	L10008
L20	equ	242
L21	equ	1
	ends
	efunc
	kdata
L19:
	db	$4F,$53,$38,$31,$36,$20,$63,$6F,$6D,$6D,$61,$6E,$64,$20,$6C
	db	$69,$6E,$65,$20,$69,$6E,$74,$65,$72,$66,$61,$63,$65,$20,$30
	db	$2E,$31,$20,$20,$28,$63,$29,$20,$32,$30,$32,$31,$20,$52,$65
	db	$69,$6E,$68,$61,$72,$64,$20,$47,$72,$61,$66,$6C,$0A,$00,$3E
	db	$00,$64,$69,$72,$00,$72,$75,$6E,$00,$74,$6F,$6B,$65,$6E,$73
	db	$00
	ends
	xref	~~strequal
	xref	~~tokenize
	xref	~~readline
	xref	~~print
