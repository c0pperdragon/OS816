;:ts=8
R0	equ	1
R1	equ	5
R2	equ	9
R3	equ	13
	code
	xdef	~~main
	func
~~main:
	longa	on
	longi	on
	tsc
	sec
	sbc	#L2
	tcs
	phd
	tcd
L10003:
	pea	#^L1
	pea	#<L1
	jsl	~~print
	pea	#<$3e8
	jsl	~~sleep
	bra	L10003
L2	equ	0
L3	equ	1
	ends
	efunc
	kdata
L1:
	db	$4D,$69,$73,$74,$21,$0A,$00
	ends
	xref	~~print
	xref	~~sleep
