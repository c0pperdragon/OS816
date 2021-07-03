;:ts=8
R0	equ	1
R1	equ	5
R2	equ	9
R3	equ	13
	code
	xdef	~~tokenize
	func
~~tokenize:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L2
	tcs
	phd
	tcd
buffer_0	set	4
tokens_0	set	8
maxtokens_0	set	12
numtokens_1	set	0
intoken_1	set	2
i_1	set	4
	stz	<L3+numtokens_1
	stz	<L3+intoken_1
	stz	<L3+i_1
	bra	L10004
L10003:
c_2	set	6
	ldy	<L3+i_1
	lda	[<L2+buffer_0],Y
	and	#$ff
	sta	<L3+c_2
	lda	<L3+c_2
	beq	L10002
	lda	#$20
	cmp	<L3+c_2
	bcc	L10006
	sep	#$20
	longa	off
	lda	#$0
	sta	[<L2+buffer_0],Y
	rep	#$20
	longa	on
	stz	<L3+intoken_1
	bra	L10001
L10006:
	lda	<L3+intoken_1
	bne	L10001
	lda	#$1
	sta	<L3+intoken_1
	lda	<L3+numtokens_1
	cmp	<L2+maxtokens_0
	bcs	L10001
	lda	<L3+numtokens_1
	asl	A
	asl	A
	clc
	adc	<L2+tokens_0
	sta	<R1
	lda	<L2+tokens_0+2
	sta	<R1+2
	lda	<L2+buffer_0
	clc
	adc	<L3+i_1
	sta	<R0
	lda	<L2+buffer_0+2
	sta	<R0+2
	lda	<R0
	sta	[<R1]
	lda	<R0+2
	ldy	#$2
	sta	[<R1],Y
	inc	<L3+numtokens_1
L10001:
	inc	<L3+i_1
L10004:
	lda	<L3+i_1
	sta	<R0
	stz	<R0+2
	lda	<R0+2
	bmi	L10003
	bne	L10002
	lda	<R0
	cmp	#65535
	bcc	L10003
L10002:
	lda	<L3+numtokens_1
	tay
	lda	<L2+2
	sta	<L2+2+10
	lda	<L2+1
	sta	<L2+1+10
	pld
	tsc
	clc
	adc	#L2+10
	tcs
	tya
	rtl
L2	equ	16
L3	equ	9
	ends
	efunc
	code
	xdef	~~strequal
	func
~~strequal:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L11
	tcs
	phd
	tcd
s1_0	set	4
s2_0	set	8
i_1	set	0
	stz	<L12+i_1
	bra	L10013
L10014:
	ldy	<L12+i_1
	lda	[<L11+s1_0],Y
	and	#$ff
	bne	L10010
	lda	#$1
	bra	L14
L10010:
	inc	<L12+i_1
L10013:
	lda	<L12+i_1
	sta	<R0
	stz	<R0+2
	lda	<R0+2
	bmi	L10012
	bne	L20000
	lda	<R0
	cmp	#65535
	bcs	L20000
L10012:
	sep	#$20
	longa	off
	ldy	<L12+i_1
	lda	[<L11+s1_0],Y
	cmp	[<L11+s2_0],Y
	rep	#$20
	longa	on
	beq	L10014
L20000:
	lda	#$0
L14:
	tay
	lda	<L11+2
	sta	<L11+2+8
	lda	<L11+1
	sta	<L11+1+8
	pld
	tsc
	clc
	adc	#L11+8
	tcs
	tya
	rtl
L11	equ	6
L12	equ	5
	ends
	efunc
