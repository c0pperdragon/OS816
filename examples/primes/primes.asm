;:ts=8
R0	equ	1
R1	equ	5
R2	equ	9
R3	equ	13
	code
	xdef	~~sieve
	func
~~sieve:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L2
	tcs
	phd
	tcd
isprime_0	set	4
printall_0	set	8
i_1	set	0
j_1	set	2
count_1	set	4
checksum_1	set	6
	stz	<L3+count_1
	stz	<L3+checksum_1
	lda	<L2+printall_0
	beq	L10001
	pea	#^L1
	pea	#<L1
	pea	#6
	jsl	~~printf
L10001:
	stz	<L3+i_1
	bra	L10005
L10004:
	sep	#$20
	longa	off
	lda	#$1
	ldy	<L3+i_1
	sta	[<L2+isprime_0],Y
	rep	#$20
	longa	on
	inc	<L3+i_1
L10005:
	lda	<L3+i_1
	sta	<R0
	stz	<R0+2
	lda	<R0+2
	bmi	L10004
	bne	L10003
	lda	<R0
	cmp	#50000
	bcc	L10004
L10003:
	lda	<L2+printall_0
	beq	L10006
	pea	#^L1+11
	pea	#<L1+11
	pea	#6
	jsl	~~printf
L10006:
	lda	#$2
	sta	<L3+i_1
	bra	L10010
L10009:
	ldy	<L3+i_1
	lda	[<L2+isprime_0],Y
	and	#$ff
	beq	L10007
	inc	<L3+count_1
	lda	<L3+checksum_1
	clc
	adc	<L3+i_1
	sta	<L3+checksum_1
	lda	<L2+printall_0
	beq	L10012
	pei	<L3+i_1
	pea	#^L1+22
	pea	#<L1+22
	pea	#8
	jsl	~~printf
L10012:
	lda	<L3+i_1
	cmp	#<$f0
	bcs	L10007
	clc
	lda	<L3+i_1
	bra	L20001
L10016:
	sep	#$20
	longa	off
	lda	#$0
	ldy	<L3+j_1
	sta	[<L2+isprime_0],Y
	rep	#$20
	longa	on
	clc
	tya
L20001:
	adc	<L3+i_1
	sta	<L3+j_1
	sta	<R0
	stz	<R0+2
	lda	<R0+2
	bmi	L10016
	bne	L10007
	lda	<R0
	cmp	#50000
	bcc	L10016
L10007:
	inc	<L3+i_1
L10010:
	lda	<L3+i_1
	sta	<R0
	stz	<R0+2
	lda	<R0+2
	bmi	L10009
	bne	L10008
	lda	<R0
	cmp	#50000
	bcc	L10009
L10008:
	lda	<L2+printall_0
	beq	L10018
	pea	#^L1+26
	pea	#<L1+26
	pea	#6
	jsl	~~printf
L10018:
	pei	<L3+checksum_1
	pei	<L3+count_1
	pea	#^L1+28
	pea	#<L1+28
	pea	#10
	jsl	~~printf
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
L2	equ	12
L3	equ	5
	ends
	efunc
	kdata
L1:
	db	$52,$65,$73,$65,$74,$74,$69,$6E,$67,$0A,$00,$43,$6F,$6D,$70
	db	$75,$74,$69,$6E,$67,$0A,$00,$25,$75,$20,$00,$0A,$00,$4E,$75
	db	$6D,$62,$65,$72,$20,$6F,$66,$20,$70,$72,$69,$6D,$65,$73,$3A
	db	$20,$25,$75,$20,$43,$68,$65,$63,$6B,$73,$75,$6D,$3A,$20,$25
	db	$75,$0A,$00
	ends
	code
	xdef	~~main
	func
~~main:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L18
	tcs
	phd
	tcd
arc_0	set	4
argv_0	set	6
line_1	set	0
buffer_1	set	10
	pea	#<$c34f
	pea	#<$2
	pea	#^L17
	pea	#<L17
	pea	#10
	jsl	~~printf
	lda	#$0
	sta	<L19+buffer_1
	ina
	ina
	sta	<L19+buffer_1+2
L10021:
	pea	#<$0
	pei	<L19+buffer_1+2
	pei	<L19+buffer_1
	jsl	~~sieve
	bra	L10021
L18	equ	14
L19	equ	1
	ends
	efunc
	kdata
L17:
	db	$43,$6F,$6D,$70,$75,$74,$69,$6E,$67,$20,$70,$72,$69,$6D,$65
	db	$73,$20,$66,$72,$6F,$6D,$20,$25,$69,$20,$74,$6F,$20,$25,$75
	db	$0A,$00
	ends
	xref	~~printf
