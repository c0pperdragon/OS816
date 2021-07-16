;:ts=8
R0	equ	1
R1	equ	5
R2	equ	9
R3	equ	13
	data
	xdef	~~checkcounter
~~checkcounter:
	dw	$0
	ends
	code
	xdef	~~test
	func
~~test:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L2
	tcs
	phd
	tcd
result_0	set	4
expected_0	set	6
	inc	|~~checkcounter
	lda	<L2+result_0
	cmp	<L2+expected_0
	bne	L10001
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
L10001:
	lda	|~~checkcounter
	pha
	jsl	~~portout
L10002:
	bra	L10002
L2	equ	0
L3	equ	1
	ends
	efunc
	code
	xdef	~~teststr
	func
~~teststr:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L6
	tcs
	phd
	tcd
result_0	set	4
expected_0	set	8
i_1	set	0
	inc	|~~checkcounter
	stz	<L7+i_1
L10007:
	sep	#$20
	longa	off
	ldy	<L7+i_1
	lda	[<L6+result_0],Y
	cmp	[<L6+expected_0],Y
	rep	#$20
	longa	on
	bne	L10006
	lda	[<L6+result_0],Y
	and	#$ff
	bne	L10005
	lda	<L6+2
	sta	<L6+2+8
	lda	<L6+1
	sta	<L6+1+8
	pld
	tsc
	clc
	adc	#L6+8
	tcs
	rtl
L10005:
	inc	<L7+i_1
	sec
	lda	<L7+i_1
	sbc	#<$2710
	bvs	L11
	eor	#$8000
L11:
	bpl	L10007
L10006:
	lda	|~~checkcounter
	pha
	jsl	~~portout
L10010:
	bra	L10010
L6	equ	2
L7	equ	1
	ends
	efunc
	code
	xdef	~~printhex
	func
~~printhex:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L13
	tcs
	phd
	tcd
buffer_0	set	4
i_1	set	0
bo_1	set	2
	stz	<L14+i_1
L10015:
c_2	set	6
	ldy	<L14+i_1
	lda	[<L13+buffer_0],Y
	and	#<$ff
	sta	<L14+c_2
	lda	<L14+c_2
	bne	L10016
	lda	|~~_iob+20
	cmp	|~~_iob+24
	lda	|~~_iob+20+2
	sbc	|~~_iob+24+2
	bcs	L16
	lda	|~~_iob+20
	sta	<R0
	lda	|~~_iob+20+2
	sta	<R0+2
	inc	|~~_iob+20
	sep	#$20
	longa	off
	lda	#$a
	sta	[<R0]
	rep	#$20
	longa	on
	lda	#$a
	and	#$ff
	bra	L19
L16:
	pea	#<$a
	lda	#<~~_iob+20
	sta	<R0
	xref	_BEG_DATA
	lda	#_BEG_DATA>>16
	pha
	pei	<R0
	jsl	~~_flsbuf
L19:
	lda	<L13+2
	sta	<L13+2+4
	lda	<L13+1
	sta	<L13+1+4
	pld
	tsc
	clc
	adc	#L13+4
	tcs
	rtl
L10016:
	lda	<L14+c_2
	sta	<R0
	asl	A
	ror	<R0
	lda	<R0
	asl	A
	ror	<R0
	lda	<R0
	asl	A
	ror	<R0
	lda	<R0
	asl	A
	ror	<R0
	lda	#$41
	clc
	adc	<R0
	pha
	jsl	~~send
	lda	<L14+c_2
	and	#<$f
	clc
	adc	#$41
	pha
	jsl	~~send
	pea	#<$20
	jsl	~~send
	inc	<L14+i_1
	sec
	lda	<L14+i_1
	sbc	#<$2710
	bvs	L20
	eor	#$8000
L20:
	bmi	*+5
	brl	L10015
	pea	#<$a
	jsl	~~send
	bra	L19
L13	equ	16
L14	equ	9
	ends
	efunc
	code
	xdef	~~test16bitoverboundaries
	func
~~test16bitoverboundaries:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L22
	tcs
	phd
	tcd
b1_1	set	0
b2_1	set	4
bt_1	set	8
	lda	#$ffff
	sta	<L23+b1_1
	lda	#$3
	sta	<L23+b1_1+2
	lda	#$0
	sta	<L23+b2_1
	lda	#$4
	sta	<L23+b2_1+2
	lda	#$ffff
	sta	<L23+bt_1
	lda	#$3
	sta	<L23+bt_1+2
	sep	#$20
	longa	off
	lda	#$47
	sta	[<L23+b1_1]
	lda	#$11
	sta	[<L23+b2_1]
	rep	#$20
	longa	on
	pea	#<$1147
	lda	[<L23+bt_1]
	pha
	jsl	~~test
	lda	#$4711
	sta	[<L23+bt_1]
	pea	#<$11
	lda	[<L23+b1_1]
	and	#$ff
	pha
	jsl	~~test
	pea	#<$47
	lda	[<L23+b2_1]
	and	#$ff
	pha
	jsl	~~test
	pld
	tsc
	clc
	adc	#L22
	tcs
	rtl
L22	equ	12
L23	equ	1
	ends
	efunc
	code
	xdef	~~testarrayaccrossboundaries
	func
~~testarrayaccrossboundaries:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L25
	tcs
	phd
	tcd
b1_1	set	0
b2_1	set	4
b3_1	set	8
	lda	#$ff00
	sta	<L26+b1_1
	lda	#$3
	sta	<L26+b1_1+2
	lda	#$fff0
	sta	<L26+b2_1
	lda	#$3
	sta	<L26+b2_1+2
	lda	#$0
	sta	<L26+b3_1
	lda	#$4
	sta	<L26+b3_1+2
	sep	#$20
	longa	off
	lda	#$63
	ldy	#$100
	sta	[<L26+b1_1],Y
	lda	#$58
	ldy	#$11
	sta	[<L26+b2_1],Y
	rep	#$20
	longa	on
	pea	#<$63
	lda	[<L26+b3_1]
	and	#$ff
	pha
	jsl	~~test
	pea	#<$58
	ldy	#$1
	lda	[<L26+b3_1],Y
	and	#$ff
	pha
	jsl	~~test
	pld
	tsc
	clc
	adc	#L25
	tcs
	rtl
L25	equ	12
L26	equ	1
	ends
	efunc
	code
	xdef	~~putbyte
	func
~~putbyte:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L28
	tcs
	phd
	tcd
buffer_0	set	4
pos_0	set	8
value_0	set	12
total_1	set	0
	lda	<L28+buffer_0
	clc
	adc	<L28+pos_0
	sta	<L29+total_1
	lda	<L28+buffer_0+2
	adc	<L28+pos_0+2
	sta	<L29+total_1+2
	sep	#$20
	longa	off
	lda	<L28+value_0
	sta	[<L29+total_1]
	rep	#$20
	longa	on
	lda	<L28+2
	sta	<L28+2+10
	lda	<L28+1
	sta	<L28+1+10
	pld
	tsc
	clc
	adc	#L28+10
	tcs
	rtl
L28	equ	4
L29	equ	1
	ends
	efunc
	code
	xdef	~~getbyte
	func
~~getbyte:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L31
	tcs
	phd
	tcd
buffer_0	set	4
pos_0	set	8
total_1	set	0
	lda	<L31+buffer_0
	clc
	adc	<L31+pos_0
	sta	<L32+total_1
	lda	<L31+buffer_0+2
	adc	<L31+pos_0+2
	sta	<L32+total_1+2
	lda	[<L32+total_1]
	and	#$ff
	tay
	lda	<L31+2
	sta	<L31+2+8
	lda	<L31+1
	sta	<L31+1+8
	pld
	tsc
	clc
	adc	#L31+8
	tcs
	tya
	rtl
L31	equ	4
L32	equ	1
	ends
	efunc
	code
	xdef	~~testlongarrayindex
	func
~~testlongarrayindex:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L34
	tcs
	phd
	tcd
b1_1	set	0
b2_1	set	4
	lda	#$0
	sta	<L35+b1_1
	lda	#$3
	sta	<L35+b1_1+2
	lda	#$ff00
	sta	<L35+b2_1
	lda	#$7
	sta	<L35+b2_1+2
	pea	#<$41
	pea	#^$4ff00
	pea	#<$4ff00
	pei	<L35+b1_1+2
	pei	<L35+b1_1
	jsl	~~putbyte
	pea	#<$41
	lda	[<L35+b2_1]
	and	#$ff
	pha
	jsl	~~test
	pea	#<$41
	pea	#^$4ff00
	pea	#<$4ff00
	pei	<L35+b1_1+2
	pei	<L35+b1_1
	jsl	~~getbyte
	sep	#$20
	longa	off
	sta	<R0
	rep	#$20
	longa	on
	lda	<R0
	and	#$ff
	pha
	jsl	~~test
	pld
	tsc
	clc
	adc	#L34
	tcs
	rtl
L34	equ	12
L35	equ	5
	ends
	efunc
	code
	xdef	~~teststrings
	func
~~teststrings:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L37
	tcs
	phd
	tcd
buffer_1	set	0
	pea	#^L1
	pea	#<L1
	pea	#0
	clc
	tdc
	adc	#<L38+buffer_1
	pha
	jsl	~~strcpy
	pea	#^L1+9
	pea	#<L1+9
	pea	#0
	clc
	tdc
	adc	#<L38+buffer_1
	pha
	jsl	~~teststr
	pea	#<$0
	pea	#^L1+18
	pea	#<L1+18
	pea	#0
	clc
	tdc
	adc	#<L38+buffer_1
	pha
	jsl	~~strcmp
	pha
	jsl	~~test
	pea	#<$1
	pea	#^L1+27
	pea	#<L1+27
	pea	#0
	clc
	tdc
	adc	#<L38+buffer_1
	pha
	jsl	~~strcmp
	pha
	jsl	~~test
	pea	#<$ffffffff
	pea	#^L1+37
	pea	#<L1+37
	pea	#0
	clc
	tdc
	adc	#<L38+buffer_1
	pha
	jsl	~~strcmp
	pha
	jsl	~~test
	pld
	tsc
	clc
	adc	#L37
	tcs
	rtl
L37	equ	100
L38	equ	1
	ends
	efunc
	kdata
L1:
	db	$48,$69,$20,$66,$6F,$6C,$6B,$73,$00,$48,$69,$20,$66,$6F,$6C
	db	$6B,$73,$00,$48,$69,$20,$66,$6F,$6C,$6B,$73,$00,$41,$6E,$64
	db	$20,$6C,$6F,$77,$65,$72,$00,$4D,$75,$63,$68,$20,$68,$69,$67
	db	$68,$65,$72,$00
	ends
	code
	xdef	~~testsprintf
	func
~~testsprintf:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L41
	tcs
	phd
	tcd
buffer_1	set	0
	pea	#^L40
	pea	#<L40
	pea	#0
	clc
	tdc
	adc	#<L42+buffer_1
	pha
	jsl	~~strcpy
	pea	#^L40+45
	pea	#<L40+45
	pea	#0
	clc
	tdc
	adc	#<L42+buffer_1
	pha
	pea	#10
	jsl	~~sprintf
	pea	#^L40+48
	pea	#<L40+48
	pea	#0
	clc
	tdc
	adc	#<L42+buffer_1
	pha
	jsl	~~teststr
	pea	#<$11
	pea	#^L40+51
	pea	#<L40+51
	pea	#0
	clc
	tdc
	adc	#<L42+buffer_1
	pha
	pea	#12
	jsl	~~sprintf
	pea	#^L40+69
	pea	#<L40+69
	pea	#0
	clc
	tdc
	adc	#<L42+buffer_1
	pha
	jsl	~~teststr
	pea	#^L40+118
	pea	#<L40+118
	pea	#<$2a
	pea	#^L40+87
	pea	#<L40+87
	pea	#0
	clc
	tdc
	adc	#<L42+buffer_1
	pha
	pea	#16
	jsl	~~sprintf
	pea	#^L40+129
	pea	#<L40+129
	pea	#0
	clc
	tdc
	adc	#<L42+buffer_1
	pha
	jsl	~~teststr
	pld
	tsc
	clc
	adc	#L41
	tcs
	rtl
L41	equ	100
L42	equ	1
	ends
	efunc
	kdata
L40:
	db	$64,$75,$6D,$6D,$79,$20,$61,$6E,$64,$20,$76,$65,$72,$79,$20
	db	$6C,$6F,$6E,$67,$20,$73,$74,$72,$69,$6E,$67,$20,$20,$20,$20
	db	$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$20,$00
	db	$66,$2A,$00,$66,$2A,$00,$42,$65,$73,$74,$20,$70,$72,$69,$6D
	db	$65,$20,$69,$73,$20,$25,$64,$2E,$00,$42,$65,$73,$74,$20,$70
	db	$72,$69,$6D,$65,$20,$69,$73,$20,$31,$37,$2E,$00,$54,$68,$65
	db	$20,$61,$6E,$73,$77,$65,$72,$20,$69,$73,$20,$25,$64,$2C,$20
	db	$77,$68,$69,$63,$68,$20,$69,$73,$20,$25,$73,$21,$00,$75,$6E
	db	$65,$78,$70,$65,$63,$74,$65,$64,$00,$54,$68,$65,$20,$61,$6E
	db	$73,$77,$65,$72,$20,$69,$73,$20,$34,$32,$2C,$20,$77,$68,$69
	db	$63,$68,$20,$69,$73,$20,$75,$6E,$65,$78,$70,$65,$63,$74,$65
	db	$64,$21,$00
	ends
	code
	xdef	~~main
	func
~~main:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L45
	tcs
	phd
	tcd
argc_0	set	4
argv_0	set	6
	jsl	~~test16bitoverboundaries
	jsl	~~testarrayaccrossboundaries
	jsl	~~testlongarrayindex
	jsl	~~teststrings
	jsl	~~testsprintf
	pea	#<$0
	jsl	~~portout
	lda	#$0
	tay
	lda	<L45+2
	sta	<L45+2+6
	lda	<L45+1
	sta	<L45+1+6
	pld
	tsc
	clc
	adc	#L45+6
	tcs
	tya
	rtl
L45	equ	0
L46	equ	1
	ends
	efunc
	xref	~~strcpy
	xref	~~strcmp
	xref	~~_flsbuf
	xref	~~sprintf
	xref	~~send
	xref	~~portout
	xref	~~_iob
