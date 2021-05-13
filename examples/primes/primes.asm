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
printall_0	set	4
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
	jsl	~~print
L10001:
	stz	<L3+i_1
	bra	L10005
L10004:
	sep	#$20
	longa	off
	lda	#$1
	ldx	<L3+i_1
	sta	|~~isprime,X
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
	jsl	~~print
L10006:
	lda	#$2
	sta	<L3+i_1
	bra	L10010
L10009:
	ldx	<L3+i_1
	lda	|~~isprime,X
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
	jsl	~~printu16
	pea	#^L1+22
	pea	#<L1+22
	jsl	~~print
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
	ldx	<L3+j_1
	sta	|~~isprime,X
	rep	#$20
	longa	on
	clc
	lda	<L3+j_1
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
	pea	#^L1+24
	pea	#<L1+24
	jsl	~~print
L10018:
	pea	#^L1+26
	pea	#<L1+26
	jsl	~~print
	pei	<L3+count_1
	jsl	~~printu16
	pea	#^L1+45
	pea	#<L1+45
	jsl	~~print
	pei	<L3+checksum_1
	jsl	~~printu16
	pea	#^L1+57
	pea	#<L1+57
	jsl	~~print
	lda	<L2+2
	sta	<L2+2+2
	lda	<L2+1
	sta	<L2+1+2
	pld
	tsc
	clc
	adc	#L2+2
	tcs
	rtl
L2	equ	12
L3	equ	5
	ends
	efunc
	kdata
L1:
	db	$52,$65,$73,$65,$74,$74,$69,$6E,$67,$0A,$00,$43,$6F,$6D,$70
	db	$75,$74,$69,$6E,$67,$0A,$00,$20,$00,$0A,$00,$4E,$75,$6D,$62
	db	$65,$72,$20,$6F,$66,$20,$70,$72,$69,$6D,$65,$73,$3A,$20,$00
	db	$20,$43,$68,$65,$63,$6B,$73,$75,$6D,$3A,$20,$00,$0A,$00
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
	pea	#^L17
	pea	#<L17
	jsl	~~print
	pea	#^L17+28
	pea	#<L17+28
	jsl	~~print
	pea	#^L17+30
	pea	#<L17+30
	jsl	~~print
	pea	#<$c34f
	jsl	~~printu16
	pea	#^L17+35
	pea	#<L17+35
	jsl	~~print
L10021:
	pea	#<$0
	jsl	~~sieve
	bra	L10021
L18	equ	0
L19	equ	1
	ends
	efunc
	kdata
L17:
	db	$43,$6F,$6D,$70,$75,$74,$65,$20,$70,$72,$69,$6D,$65,$20,$6E
	db	$75,$6D,$62,$65,$72,$73,$20,$66,$72,$6F,$6D,$20,$00,$32,$00
	db	$20,$74,$6F,$20,$00,$0A,$00
	ends
	xref	~~printu16
	xref	~~print
	udata
	xdef	~~isprime
~~isprime
	ds	50000
	ends
