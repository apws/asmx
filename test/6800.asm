	PROCESSOR 6800

	FCB	$00
	NOP
	FCB	$02
	FCB	$03
	FCB	$04
	FCB	$05
	TAP
	TPA
	INX
	DEX
	CLV
	SEV
	CLC
	SEC
	CLI
	SEI

	SBA
	CBA
	FCB	$12
	FCB	$13
	FCB	$14
	FCB	$15
	TAB
	TBA
	FCB	$18
	DAA
	FCB	$1A
	ABA
	FCB	$1C
	FCB	$1D
	FCB	$1E
	FCB	$1F

	BRA	.+2
	FCB	$21
	BHI	.+2
	BLS	.+2
	BCC	.+2		; also BHS
	BCS	.+2		; also BLO
	BNE	.+2
	BEQ	.+2
	BVC	.+2
	BVS	.+2
	BPL	.+2
	BMI	.+2
	BGE	.+2
	BLT	.+2
	BGT	.+2
	BLE	.+2

	TSX
	INS
	PULA
	PULB
	DES
	TXS
	PSHA
	PSHB
	FCB	$38
	RTS
	FCB	$3A
	RTI
	FCB	$3C
	FCB	$3D
	WAI
	SWI

	NEGA
	FCB	$41
	FCB	$42
	COMA
	LSRA
	FCB	$45
	RORA
	ASRA
	ASLA
	ROLA
	DECA
	FCB	$4B
	INCA
	TSTA
	FCB	$4E
	CLRA

	NEGB
	FCB	$51
	FCB	$52
	COMB
	LSRB
	FCB	$55
	RORB
	ASRB
	ASLB
	ROLB
	DECB
	FCB	$5B
	INCB
	TSTB
	FCB	$5E
	CLRB

	NEG	$58,X
	FCB	$61
	FCB	$62
	COM	$58,X
	LSR	$58,X
	FCB	$65
	ROR	$58,X
	ASR	$58,X
	ASL	$58,X
	ROL	$58,X
	DEC	$58,X
	FCB	$6B
	INC	$58,X
	TST	$58,X
	JMP	$58,X
	CLR	$58,X

	NEG	$4558
	FCB	$71
	FCB	$72
	COM	$4558
	LSR	$4558
	FCB	$75
	ROR	$4558
	ASR	$4558
	ASL	$4558
	ROL	$4558
	DEC	$4558
	FCB	$7B
	INC	$4558
	TST	$4558
	JMP	.+3
	CLR	$4558

	SUBA	#$49
	CMPA	#$49
	SBCA	#$49
	FCB	$83
	ANDA	#$49
	BITA	#$49
	LDAA	#$49
	FCB	$87
	EORA	#$49
	ADCA	#$49
	ORAA	#$49
	ADDA	#$49
	CPX	#$494D
	BSR	.+2
	LDS	#$494D
	FCB	$8F

	SUBA	$44
	CMPA	$44
	SBCA	$44
	FCB	$93
	ANDA	$44
	BITA	$44
	LDAA	$44
	STAA	$44
	EORA	$44
	ADCA	$44
	ORAA	$44
	ADDA	$44
	CPX	$44
	FCB	$9D
	LDS	$44
	STS	$44

	SUBA	$58,X
	CMPA	$58,X
	SBCA	$58,X
	FCB	$A3
	ANDA	$58,X
	BITA	$58,X
	LDAA	$58,X
	STAA	$58,X
	EORA	$58,X
	ADCA	$58,X
	ORAA	$58,X
	ADDA	$58,X
	CPX	$58,X
	JSR	$58,X
	LDS	$58,X
	STS	$58,X

	SUBA	$4558
	CMPA	$4558
	SBCA	$4558
	FCB	$B3
	ANDA	$4558
	BITA	$4558
	LDAA	$4558
	STAA	$4558
	EORA	$4558
	ADCA	$4558
	ORAA	$4558
	ADDA	$4558
	CPX	$4558
	JSR	.+3
	LDS	$4558
	STS	$4558

	SUBB	#$49
	CMPB	#$49
	SBCB	#$49
	FCB	$C3
	ANDB	#$49
	BITB	#$49
	LDAB	#$49
	FCB	$C7
	EORB	#$49
	ADCB	#$49
	ORAB	#$49
	ADDB	#$49
	FCB	$CC
	FCB	$CD
	LDX	#$494D
	FCB	$BF

	SUBB	$44
	CMPB	$44
	SBCB	$44
	FCB	$C3
	ANDB	$44
	BITB	$44
	LDAB	$44
	STAB	$44
	EORB	$44
	ADCB	$44
	ORAB	$44
	ADDB	$44
	FCB	$DC
	FCB	$DD
	LDX	$44
	STX	$44

	SUBB	$58,X
	CMPB	$58,X
	SBCB	$58,X
	FCB	$E3
	ANDB	$58,X
	BITB	$58,X
	LDAB	$58,X
	STAB	$58,X
	EORB	$58,X
	ADCB	$58,X
	ORAB	$58,X
	ADDB	$58,X
	FCB	$EC
	FCB	$ED
	LDX	$58,X
	STX	$58,X

	SUBB	$4558
	CMPB	$4558
	SBCB	$4558
	FCB	$F3
	ANDB	$4558
	BITB	$4558
	LDAB	$4558
	STAB	$4558
	EORB	$4558
	ADCB	$4558
	ORAB	$4558
	ADDB	$4558
	FCB	$FC
	FCB	$FD
	LDX	$4558
	STX	$4558
