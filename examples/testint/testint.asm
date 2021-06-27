;:ts=8
R0	equ	1
R1	equ	5
R2	equ	9
R3	equ	13
	code
	xdef	~~putbyte
	func
~~putbyte:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L2
	tcs
	phd
	tcd
buffer_0	set	4
pos_0	set	8
value_0	set	12
	sep	#$20
	longa	off
	lda	<L2+value_0
	ldy	<L2+pos_0
	sta	[<L2+buffer_0],Y
	rep	#$20
	longa	on
	lda	<L2+2
	sta	<L2+2+10
	lda	<L2+1
	sta	<L2+1+10
	pld
	tsc
	clc
	adc	#L2+10
	tcs
	rtl
L2	equ	0
L3	equ	1
	ends
	efunc
	code
	xdef	~~prepare
	func
~~prepare:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L5
	tcs
	phd
	tcd
buffer_0	set	4
	pea	#<$4d
	pea	#^$0
	pea	#<$0
	pei	<L5+buffer_0+2
	pei	<L5+buffer_0
	jsl	~~putbyte
	pea	#<$69
	pea	#^$1
	pea	#<$1
	pei	<L5+buffer_0+2
	pei	<L5+buffer_0
	jsl	~~putbyte
	pea	#<$73
	pea	#^$2
	pea	#<$2
	pei	<L5+buffer_0+2
	pei	<L5+buffer_0
	jsl	~~putbyte
	pea	#<$74
	pea	#^$3
	pea	#<$3
	pei	<L5+buffer_0+2
	pei	<L5+buffer_0
	jsl	~~putbyte
	pea	#<$0
	pea	#^$4
	pea	#<$4
	pei	<L5+buffer_0+2
	pei	<L5+buffer_0
	jsl	~~putbyte
	lda	<L5+2
	sta	<L5+2+4
	lda	<L5+1
	sta	<L5+1+4
	pld
	tsc
	clc
	adc	#L5+4
	tcs
	rtl
L5	equ	0
L6	equ	1
	ends
	efunc
	code
	xdef	~~subl
	func
~~subl:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L8
	tcs
	phd
	tcd
a_0	set	4
b_0	set	8
	sec
	lda	<L8+a_0
	sbc	<L8+b_0
	sta	<R0
	lda	<L8+a_0+2
	sbc	<L8+b_0+2
	sta	<R0+2
	lda	<R0
	tay
	lda	<L8+2
	sta	<L8+2+8
	lda	<L8+1
	sta	<L8+1+8
	pld
	tsc
	clc
	adc	#L8+8
	tcs
	tya
	rtl
L8	equ	4
L9	equ	5
	ends
	efunc
	code
	xdef	~~printindirect
	func
~~printindirect:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L11
	tcs
	phd
	tcd
p_0	set	4
	lda	[<L11+p_0]
	pha
	jsl	~~printu16
	lda	<L11+2
	sta	<L11+2+4
	lda	<L11+1
	sta	<L11+1+4
	pld
	tsc
	clc
	adc	#L11+4
	tcs
	rtl
L11	equ	0
L12	equ	1
	ends
	efunc
	code
	xdef	~~main
	func
~~main:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L14
	tcs
	phd
	tcd
buffer_1	set	0
extension_1	set	4
extension2_1	set	8
	lda	#$0
	sta	<L15+buffer_1
	ina
	ina
	sta	<L15+buffer_1+2
	dea
	dea
	sta	<L15+extension_1
	lda	#$3
	sta	<L15+extension_1+2
	lda	#$0
	sta	<L15+extension2_1
	lda	#$4
	sta	<L15+extension2_1+2
	sep	#$20
	longa	off
	lda	#$40
	sta	[<L15+extension_1]
	lda	#$0
	ldy	#$1
	sta	[<L15+extension_1],Y
	rep	#$20
	longa	on
	lda	#$fffe
	clc
	adc	<L15+buffer_1
	sta	<R0
	lda	<L15+buffer_1+2
	pha
	pei	<R0
	jsl	~~prepare
	lda	#$ffff
	clc
	adc	<L15+extension_1
	sta	<R1
	lda	<L15+extension_1+2
	sta	<R1+2
	sep	#$20
	longa	off
	lda	#$2f
	sta	[<R1]
	lda	#$b
	sta	[<L15+extension2_1]
	rep	#$20
	longa	on
	pea	#^L1
	pea	#<L1
	jsl	~~print
	pea	#^$c4ef0a0
	pea	#<$c4ef0a0
	pea	#^$c4f9e25
	pea	#<$c4f9e25
	jsl	~~subl
	pha
	jsl	~~printu16
	pea	#^L1+2
	pea	#<L1+2
	jsl	~~print
L10003:
	lda	[<L15+buffer_1]
	and	#$ff
	pha
	jsl	~~printu16
	pea	#^L1+4
	pea	#<L1+4
	jsl	~~print
	ldy	#$1
	lda	[<L15+buffer_1],Y
	and	#$ff
	pha
	jsl	~~printu16
	pea	#^L1+6
	pea	#<L1+6
	jsl	~~print
	lda	#$fffe
	clc
	adc	<L15+buffer_1
	sta	<R0
	lda	<L15+buffer_1+2
	sta	<R0+2
	lda	[<R0]
	and	#$ff
	pha
	jsl	~~printu16
	pea	#^L1+8
	pea	#<L1+8
	jsl	~~print
	lda	#$ffff
	clc
	adc	<L15+buffer_1
	sta	<R0
	lda	<L15+buffer_1+2
	sta	<R0+2
	lda	[<R0]
	and	#$ff
	pha
	jsl	~~printu16
	pea	#^L1+10
	pea	#<L1+10
	jsl	~~print
	lda	[<L15+extension_1]
	and	#$ff
	pha
	jsl	~~printu16
	pea	#^L1+12
	pea	#<L1+12
	jsl	~~print
	ldy	#$1
	lda	[<L15+extension_1],Y
	and	#$ff
	pha
	jsl	~~printu16
	pea	#^L1+14
	pea	#<L1+14
	jsl	~~print
	lda	#$fffe
	clc
	adc	<L15+buffer_1
	sta	<R0
	lda	<L15+buffer_1+2
	pha
	pei	<R0
	jsl	~~print
	pea	#^L1+16
	pea	#<L1+16
	jsl	~~print
	lda	#$ffff
	clc
	adc	<L15+extension_1
	sta	<R0
	lda	<L15+extension_1+2
	pha
	pei	<R0
	jsl	~~printindirect
	pea	#^L1+18
	pea	#<L1+18
	jsl	~~print
	pea	#<$3e8
	jsl	~~sleep
	brl	L10003
L14	equ	20
L15	equ	9
	ends
	efunc
	kdata
L1:
	db	$0A,$00,$0A,$00,$20,$00,$20,$00,$20,$00,$20,$00,$20,$00,$20
	db	$00,$20,$00,$0A,$00
	ends
	xref	~~printu16
	xref	~~print
	xref	~~sleep
