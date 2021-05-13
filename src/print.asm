;:ts=8
R0	equ	1
R1	equ	5
R2	equ	9
R3	equ	13
	code
	xdef	~~print
	func
~~print:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L2
	tcs
	phd
	tcd
text_0	set	4
L10003:
c_2	set	0
	sep	#$20
	longa	off
	lda	[<L2+text_0]
	sta	<L3+c_2
	rep	#$20
	longa	on
	lda	<L3+c_2
	and	#$ff
	bne	L10004
	lda	<L2+2
	sta	<L2+2+4
	lda	<L2+1
	sta	<L2+1+4
	pld
	tsc
	clc
	adc	#L2+4
	tcs
	rtl
L10004:
	lda	<L3+c_2
	and	#$ff
	pha
	jsl	~~send
	inc	<L2+text_0
	bra	L10003
L2	equ	1
L3	equ	1
	ends
	efunc
	kdata
	xdef	~~digitvalues
~~digitvalues:
	dw	$2710,$3E8,$64,$A
	ends
	code
	xdef	~~printu16
	func
~~printu16:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L6
	tcs
	phd
	tcd
value_0	set	4
haveleading_1	set	0
pos_1	set	2
	stz	<L7+haveleading_1
	stz	<L7+pos_1
L10007:
digitvalue_2	set	4
digit_2	set	6
	lda	<L7+pos_1
	asl	A
	tax
	lda	>~~digitvalues,X
	sta	<L7+digitvalue_2
	stz	<L7+digit_2
	bra	L10008
L20001:
	sec
	lda	<L6+value_0
	sbc	<L7+digitvalue_2
	sta	<L6+value_0
	inc	<L7+digit_2
L10008:
	lda	<L6+value_0
	cmp	<L7+digitvalue_2
	bcs	L20001
	lda	#$0
	cmp	<L7+digit_2
	bcc	L9
	lda	<L7+haveleading_1
	beq	L10005
L9:
	lda	#$1
	sta	<L7+haveleading_1
	lda	#$30
	clc
	adc	<L7+digit_2
	pha
	jsl	~~send
L10005:
	inc	<L7+pos_1
	lda	<L7+pos_1
	cmp	#<$4
	bcc	L10007
	lda	#$30
	clc
	adc	<L6+value_0
	pha
	jsl	~~send
	lda	<L6+2
	sta	<L6+2+2
	lda	<L6+1
	sta	<L6+1+2
	pld
	tsc
	clc
	adc	#L6+2
	tcs
	rtl
L6	equ	12
L7	equ	5
	ends
	efunc
	xref	~~send
