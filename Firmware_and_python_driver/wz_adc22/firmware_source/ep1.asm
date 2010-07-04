;--------------------------------------------------------
; File Created by SDCC : FreeWare ANSI-C Compiler
; Version 2.6.0 #4309 (Aug 16 2006)
; This file generated Fri Sep 15 11:39:29 2006
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4455

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _ep1_init
	global _ep1_out
	global _ep1_OutBuffer
	global _id_msg
	global _ack
	global _nack

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern __gptrput1
	extern _ep_bdt
	extern _usb_device_state
	extern _usb_active_cfg
	extern _usb_active_alt_setting
	extern _device_descriptor
	extern _configuration_descriptor
	extern _string_descriptor
	extern _ep_init
	extern _ep_in
	extern _ep_out
	extern _ep_setup
	extern _ep2_num_bytes_to_send
	extern _ep2_source_data
	extern _SPPDATAbits
	extern _SPPCFGbits
	extern _SPPEPSbits
	extern _SPPCONbits
	extern _UFRMLbits
	extern _UFRMHbits
	extern _UIRbits
	extern _UIEbits
	extern _UEIRbits
	extern _UEIEbits
	extern _USTATbits
	extern _UCONbits
	extern _UADDRbits
	extern _UCFGbits
	extern _UEP0bits
	extern _UEP1bits
	extern _UEP2bits
	extern _UEP3bits
	extern _UEP4bits
	extern _UEP5bits
	extern _UEP6bits
	extern _UEP7bits
	extern _UEP8bits
	extern _UEP9bits
	extern _UEP10bits
	extern _UEP11bits
	extern _UEP12bits
	extern _UEP13bits
	extern _UEP14bits
	extern _UEP15bits
	extern _PORTAbits
	extern _PORTBbits
	extern _PORTCbits
	extern _PORTDbits
	extern _PORTEbits
	extern _LATAbits
	extern _LATBbits
	extern _LATCbits
	extern _LATDbits
	extern _LATEbits
	extern _TRISAbits
	extern _TRISBbits
	extern _TRISCbits
	extern _TRISDbits
	extern _TRISEbits
	extern _OSCTUNEbits
	extern _PIE1bits
	extern _PIR1bits
	extern _IPR1bits
	extern _PIE2bits
	extern _PIR2bits
	extern _IPR2bits
	extern _EECON1bits
	extern _RCSTAbits
	extern _TXSTAbits
	extern _T3CONbits
	extern _CMCONbits
	extern _CVRCONbits
	extern _ECCP1ASbits
	extern _ECCP1DELbits
	extern _BAUDCONbits
	extern _CCP2CONbits
	extern _ECCP1CONbits
	extern _ADCON2bits
	extern _ADCON1bits
	extern _ADCON0bits
	extern _SSPCON2bits
	extern _SSPCON1bits
	extern _SSPSTATbits
	extern _T2CONbits
	extern _T1CONbits
	extern _RCONbits
	extern _WDTCONbits
	extern _HLVDCONbits
	extern _OSCCONbits
	extern _T0CONbits
	extern _STATUSbits
	extern _FSR2Hbits
	extern _BSRbits
	extern _FSR1Hbits
	extern _FSR0Hbits
	extern _INTCON3bits
	extern _INTCON2bits
	extern _INTCONbits
	extern _TBLPTRUbits
	extern _PCLATHbits
	extern _PCLATUbits
	extern _STKPTRbits
	extern _TOSUbits
	extern _adc_cfg
	extern _adc_int_nr
	extern _adc_flags
	extern _SPPDATA
	extern _SPPCFG
	extern _SPPEPS
	extern _SPPCON
	extern _UFRML
	extern _UFRMH
	extern _UIR
	extern _UIE
	extern _UEIR
	extern _UEIE
	extern _USTAT
	extern _UCON
	extern _UADDR
	extern _UCFG
	extern _UEP0
	extern _UEP1
	extern _UEP2
	extern _UEP3
	extern _UEP4
	extern _UEP5
	extern _UEP6
	extern _UEP7
	extern _UEP8
	extern _UEP9
	extern _UEP10
	extern _UEP11
	extern _UEP12
	extern _UEP13
	extern _UEP14
	extern _UEP15
	extern _PORTA
	extern _PORTB
	extern _PORTC
	extern _PORTD
	extern _PORTE
	extern _LATA
	extern _LATB
	extern _LATC
	extern _LATD
	extern _LATE
	extern _TRISA
	extern _TRISB
	extern _TRISC
	extern _TRISD
	extern _TRISE
	extern _OSCTUNE
	extern _PIE1
	extern _PIR1
	extern _IPR1
	extern _PIE2
	extern _PIR2
	extern _IPR2
	extern _EECON1
	extern _EECON2
	extern _EEDATA
	extern _EEADR
	extern _RCSTA
	extern _TXSTA
	extern _TXREG
	extern _RCREG
	extern _SPBRG
	extern _SPBRGH
	extern _T3CON
	extern _TMR3L
	extern _TMR3H
	extern _CMCON
	extern _CVRCON
	extern _ECCP1AS
	extern _ECCP1DEL
	extern _BAUDCON
	extern _CCP2CON
	extern _CCPR2L
	extern _CCPR2H
	extern _ECCP1CON
	extern _CCPR1L
	extern _CCPR1H
	extern _ADCON2
	extern _ADCON1
	extern _ADCON0
	extern _ADRESL
	extern _ADRESH
	extern _SSPCON2
	extern _SSPCON1
	extern _SSPSTAT
	extern _SSPADD
	extern _SSPBUF
	extern _T2CON
	extern _PR2
	extern _TMR2
	extern _T1CON
	extern _TMR1L
	extern _TMR1H
	extern _RCON
	extern _WDTCON
	extern _HLVDCON
	extern _OSCCON
	extern _T0CON
	extern _TMR0L
	extern _TMR0H
	extern _STATUS
	extern _FSR2L
	extern _FSR2H
	extern _PLUSW2
	extern _PREINC2
	extern _POSTDEC2
	extern _POSTINC2
	extern _INDF2
	extern _BSR
	extern _FSR1L
	extern _FSR1H
	extern _PLUSW1
	extern _PREINC1
	extern _POSTDEC1
	extern _POSTINC1
	extern _INDF1
	extern _WREG
	extern _FSR0L
	extern _FSR0H
	extern _PLUSW0
	extern _PREINC0
	extern _POSTDEC0
	extern _POSTINC0
	extern _INDF0
	extern _INTCON3
	extern _INTCON2
	extern _INTCON
	extern _PRODL
	extern _PRODH
	extern _TABLAT
	extern _TBLPTRL
	extern _TBLPTRH
	extern _TBLPTRU
	extern _PCL
	extern _PCLATH
	extern _PCLATU
	extern _STKPTR
	extern _TOSL
	extern _TOSH
	extern _TOSU
	extern _prepare_ep2_in
	extern _adc_start
	extern _adc_init
	extern _adc_stop
	extern _boot_device_descriptor
	extern _boot_configuration_descriptor
	extern _boot_string_descriptor
	extern _str0
	extern _str1
	extern _str2
	extern _str3
	extern _boot_ep_init
	extern _boot_ep_in
	extern _boot_ep_out
	extern _boot_ep_setup
	extern _section_descriptor
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
STATUS	equ	0xfd8
PCL	equ	0xff9
PCLATH	equ	0xffa
PCLATU	equ	0xffb
WREG	equ	0xfe8
FSR0L	equ	0xfe9
FSR0H	equ	0xfea
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
INDF0	equ	0xfef
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4
PRODL	equ	0xff3


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1
r0x02	res	1
r0x03	res	1
r0x04	res	1
r0x05	res	1
r0x06	res	1
r0x07	res	1
r0x08	res	1
r0x09	res	1

udata_ep1_0	udata
iTemp0	res	1

udata_ep1_1	udata
iTemp13	res	1


ustat_ep1_00	udata	0X0510
_ep1_OutBuffer	res	64

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_ep1__ep1_out	code
_ep1_out:
;	.line	49; ep1.c	void ep1_out(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	MOVFF	r0x04, POSTDEC1
	MOVFF	r0x05, POSTDEC1
	MOVFF	r0x06, POSTDEC1
	MOVFF	r0x07, POSTDEC1
;	.line	52; ep1.c	if(EP_OUT_BD(1).Cnt >= 1)
	MOVFF	(_ep_bdt + 9), (iTemp0 + 1)
	MOVLW	0x01
	BANKSEL	(iTemp0 + 1)
	SUBWF	(iTemp0 + 1), W, B
	BTFSS	STATUS, 0
	GOTO	_00130_DS_
;	.line	54; ep1.c	switch(ep1_OutBuffer[0])
	LFSR	0x00, _ep1_OutBuffer
	MOVFF	INDF0, r0x00
	MOVLW	0x06
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	GOTO	_00130_DS_
	MOVFF	r0x08, POSTDEC1
	MOVFF	r0x09, POSTDEC1
	CLRF	r0x09
	RLCF	r0x00, W
	RLCF	r0x09, F
	RLCF	WREG, W
	RLCF	r0x09, F
	ANDLW	0xfc
	MOVWF	r0x08
	MOVLW	UPPER(_00147_DS_)
	MOVWF	PCLATU
	MOVLW	HIGH(_00147_DS_)
	MOVWF	PCLATH
	MOVLW	LOW(_00147_DS_)
	ADDWF	r0x08, F
	MOVF	r0x09, W
	ADDWFC	PCLATH, F
	BTFSC	STATUS, 0
	INCF	PCLATU, F
	MOVF	r0x08, W
	MOVFF	PREINC1, r0x09
	MOVFF	PREINC1, r0x08
	MOVWF	PCL
_00147_DS_:
	GOTO	_00109_DS_
	GOTO	_00110_DS_
	GOTO	_00115_DS_
	GOTO	_00119_DS_
	GOTO	_00120_DS_
	GOTO	_00121_DS_
_00109_DS_:
;	.line	59; ep1.c	ep2_num_bytes_to_send = sizeof(id_msg);
	MOVLW	0x11
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	60; ep1.c	ep2_source_data = (uchar __code *) id_msg;
	MOVLW	LOW(_id_msg)
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	HIGH(_id_msg)
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVLW	UPPER(_id_msg)
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	61; ep1.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
;	.line	62; ep1.c	break;
	GOTO	_00123_DS_
_00110_DS_:
;	.line	65; ep1.c	if(EP_OUT_BD(1).Cnt < ADC_CONF_SIZE+1) {
	MOVFF	(_ep_bdt + 9), (iTemp13 + 1)
	MOVLW	0x0c
	BANKSEL	(iTemp13 + 1)
	SUBWF	(iTemp13 + 1), W, B
	BTFSS	STATUS, 0
	GOTO	_00130_DS_
;	.line	68; ep1.c	if(adc_flags.adc_sampling == 1) {
	CLRF	r0x00
	BANKSEL	_adc_flags
	BTFSC	_adc_flags, 0, B
	INCF	r0x00, F
	MOVF	r0x00, W
	XORLW	0x01
	BNZ	_00150_DS_
	GOTO	_00130_DS_
_00150_DS_:
;	.line	72; ep1.c	for(j=0;j<sizeof(adc_cfg);j++) {
	CLRF	r0x00
_00131_DS_:
	MOVLW	0x0b
	SUBWF	r0x00, W
	BTFSC	STATUS, 0
	GOTO	_00134_DS_
;	.line	73; ep1.c	*(((uchar *) &adc_cfg)+j) = ep1_OutBuffer[j+1];
	MOVLW	HIGH(_adc_cfg)
	MOVWF	r0x02
	MOVLW	LOW(_adc_cfg)
	MOVWF	r0x01
	MOVLW	0x80
	MOVWF	r0x03
	MOVF	r0x00, W
	ADDWF	r0x01, F
	CLRF	WREG
	ADDWFC	r0x02, F
	CLRF	WREG
	ADDWFC	r0x03, F
	INCF	r0x00, W
	MOVWF	r0x04
	MOVLW	LOW(_ep1_OutBuffer)
	ADDWF	r0x04, W
	MOVWF	r0x05
	CLRF	r0x06
	MOVLW	HIGH(_ep1_OutBuffer)
	ADDWFC	r0x06, F
	CLRF	r0x07
	MOVLW	UPPER(_ep1_OutBuffer)
	ADDWFC	r0x07, F
	MOVFF	r0x05, FSR0L
	MOVFF	r0x06, FSR0H
	MOVFF	INDF0, r0x05
	MOVFF	r0x05, POSTDEC1
	MOVFF	r0x01, FSR0L
	MOVFF	r0x02, PRODL
	MOVF	r0x03, W
	CALL	__gptrput1
;	.line	72; ep1.c	for(j=0;j<sizeof(adc_cfg);j++) {
	MOVFF	r0x04, r0x00
	GOTO	_00131_DS_
_00134_DS_:
;	.line	75; ep1.c	ep2_num_bytes_to_send = sizeof(ack);
	MOVLW	0x03
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	76; ep1.c	ep2_source_data = (uchar __code *) ack;
	MOVLW	LOW(_ack)
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	HIGH(_ack)
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVLW	UPPER(_ack)
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	77; ep1.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
;	.line	78; ep1.c	break;
	GOTO	_00123_DS_
_00115_DS_:
	BANKSEL	_adc_flags
;	.line	81; ep1.c	if(adc_flags.adc_sampling==0) {
	BTFSC	_adc_flags, 0, B
	GOTO	_00130_DS_
;	.line	82; ep1.c	adc_init();
	CALL	_adc_init
;	.line	83; ep1.c	adc_start();
	CALL	_adc_start
;	.line	84; ep1.c	ep2_num_bytes_to_send = sizeof(ack);
	MOVLW	0x03
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	85; ep1.c	ep2_source_data = (uchar __code *) ack;
	MOVLW	LOW(_ack)
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	HIGH(_ack)
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVLW	UPPER(_ack)
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	86; ep1.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
	GOTO	_00123_DS_
_00119_DS_:
;	.line	93; ep1.c	adc_stop();
	CALL	_adc_stop
;	.line	94; ep1.c	ep2_num_bytes_to_send = sizeof(ack);
	MOVLW	0x03
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	95; ep1.c	ep2_source_data = (uchar __code *) ack;
	MOVLW	LOW(_ack)
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	HIGH(_ack)
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVLW	UPPER(_ack)
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	96; ep1.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
;	.line	97; ep1.c	break;
	GOTO	_00123_DS_
_00120_DS_:
;	.line	100; ep1.c	adc_stop();
	CALL	_adc_stop
;	.line	101; ep1.c	ep2_num_bytes_to_send =2;
	MOVLW	0x02
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	102; ep1.c	ep2_source_data = &adc_int_nr;
	MOVLW	HIGH(_adc_int_nr)
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVLW	LOW(_adc_int_nr)
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	0x80
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	103; ep1.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
;	.line	104; ep1.c	break;
	GOTO	_00123_DS_
_00121_DS_:
;	.line	107; ep1.c	adc_stop();
	CALL	_adc_stop
;	.line	108; ep1.c	ep2_num_bytes_to_send =sizeof(adc_cfg);
	MOVLW	0x0b
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	109; ep1.c	ep2_source_data = &adc_cfg;
	MOVLW	HIGH(_adc_cfg)
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVLW	LOW(_adc_cfg)
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	0x80
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	110; ep1.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
_00123_DS_:
;	.line	117; ep1.c	EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
	MOVLW	0x40
	BANKSEL	(_ep_bdt + 9)
	MOVWF	(_ep_bdt + 9), B
	BANKSEL	(_ep_bdt + 8)
;	.line	118; ep1.c	if(EP_OUT_BD(1).Stat.DTS == 0)
	BTFSC	(_ep_bdt + 8), 6, B
	GOTO	_00125_DS_
;	.line	120; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT1 | BDS_DTSEN;
	MOVLW	0xc8
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
	GOTO	_00129_DS_
_00125_DS_:
;	.line	124; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	0x88
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
_00129_DS_:
;	.line	131; ep1.c	return;
	GOTO	_00135_DS_
_00130_DS_:
;	.line	133; ep1.c	EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
	MOVLW	0x40
	BANKSEL	(_ep_bdt + 9)
	MOVWF	(_ep_bdt + 9), B
;	.line	134; ep1.c	EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
	MOVLW	0x10
	BANKSEL	(_ep_bdt + 10)
	MOVWF	(_ep_bdt + 10), B
	MOVLW	0x05
	BANKSEL	(_ep_bdt + 11)
	MOVWF	(_ep_bdt + 11), B
;	.line	135; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_BSTALL;
	MOVLW	0x84
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
_00135_DS_:
;	.line	136; ep1.c	return;
	MOVFF	PREINC1, r0x07
	MOVFF	PREINC1, r0x06
	MOVFF	PREINC1, r0x05
	MOVFF	PREINC1, r0x04
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_ep1__ep1_init	code
_ep1_init:
;	.line	40; ep1.c	void ep1_init(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	43; ep1.c	EP_OUT_BD(1).Cnt = EP1_BUFFER_SIZE;
	MOVLW	0x40
	BANKSEL	(_ep_bdt + 9)
	MOVWF	(_ep_bdt + 9), B
;	.line	44; ep1.c	EP_OUT_BD(1).ADR = (uchar __data *)&ep1_OutBuffer;
	MOVLW	0x10
	BANKSEL	(_ep_bdt + 10)
	MOVWF	(_ep_bdt + 10), B
	MOVLW	0x05
	BANKSEL	(_ep_bdt + 11)
	MOVWF	(_ep_bdt + 11), B
;	.line	45; ep1.c	EP_OUT_BD(1).Stat.uc = BDS_USIE | BDS_DAT0 | BDS_DTSEN;
	MOVLW	0x88
	BANKSEL	(_ep_bdt + 8)
	MOVWF	(_ep_bdt + 8), B
;	.line	46; ep1.c	UEP1 = EPHSHK_EN | EPOUTEN_EN | EPCONDIS_EN;       // Init EP1 as an OUT EP
	MOVLW	0x1c
	MOVWF	_UEP1
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block for Ival
	code
_id_msg:
	DB	0x55, 0x53, 0x42, 0x20, 0x41, 0x44, 0x43, 0x20, 0x76, 0x65, 0x72, 0x2e
	DB	0x20, 0x31, 0x2e, 0x31, 0x00
; ; Starting pCode block for Ival
_ack:
	DB	0x4f, 0x4b, 0x00
; ; Starting pCode block for Ival
_nack:
	DB	0x45, 0x52, 0x52, 0x4f, 0x52, 0x00


; Statistics:
; code size:	  670 (0x029e) bytes ( 0.51%)
;           	  335 (0x014f) words
; udata size:	   66 (0x0042) bytes ( 3.68%)
; access size:	   10 (0x000a) bytes


	end
