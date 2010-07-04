;--------------------------------------------------------
; File Created by SDCC : FreeWare ANSI-C Compiler
; Version 2.6.0 #4309 (Aug 16 2006)
; This file generated Fri Sep 15 11:51:33 2006
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4455

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _adc_init
	global _adc_reset
	global _adc_start
	global _adc_stop
	global _adc_dispatch
	global _adc_flags
	global _adc_cfg
	global _adc_int_nr
	global _adc_interrupt
	global _err_ovr

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
	extern __divuchar
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
PCLATH	equ	0xffa
PCLATU	equ	0xffb
WREG	equ	0xfe8
BSR	equ	0xfe0
FSR0L	equ	0xfe9
FSR0H	equ	0xfea
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
INDF0	equ	0xfef
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4
PRODL	equ	0xff3
PRODH	equ	0xff4


	idata
_adc_data_len	db	0x00
_adc_rec_nr	db	0x00
_adc_flags	db	0x00
_adc_buf_ptr	db	0x00
_adc_int_nr	db	0x64, 0x00
_adc_one_buf_size	db	0x00


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1
r0x02	res	1
r0x03	res	1

udata_adc_0	udata
_adc_nbufs	res	1

udata_adc_1	udata
_adc_in	res	1

udata_adc_2	udata
_adc_out	res	1

udata_adc_3	udata
_adc_nchans	res	1

udata_adc_4	udata
_adc_nchan	res	1

udata_adc_5	udata
_adc_interrupt_tmp_1_1	res	1

udata_adc_6	udata
_adc_cfg	res	11

udata_adc_7	udata
_adc_buf	res	255

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; ; Starting pCode block for absolute section
; ;-----------------------------------------
S_adc_ivec_0x1_adc_interrupt	code	0X000008
ivec_0x1_adc_interrupt:
	GOTO	_adc_interrupt

; I code from now on!
; ; Starting pCode block
S_adc__adc_dispatch	code
_adc_dispatch:
;	.line	133; adc.c	void adc_dispatch(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	BANKSEL	(_ep_bdt + 20)
;	.line	138; adc.c	if( !(EP_IN_BD(2).Stat.uc & BDS_USIE)) {
	BTFSC	(_ep_bdt + 20), 7
	GOTO	_00163_DS_
	BANKSEL	_adc_flags
;	.line	140; adc.c	if(adc_flags.adc_overrun) {
	BTFSS	_adc_flags, 1, B
	GOTO	_00159_DS_
;	.line	141; adc.c	ep2_num_bytes_to_send = sizeof(err_ovr);
	MOVLW	0x0a
	BANKSEL	_ep2_num_bytes_to_send
	MOVWF	_ep2_num_bytes_to_send, B
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
;	.line	142; adc.c	ep2_source_data = err_ovr;
	MOVLW	LOW(_err_ovr)
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	HIGH(_err_ovr)
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVLW	UPPER(_err_ovr)
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	143; adc.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
	BANKSEL	_adc_flags
;	.line	144; adc.c	adc_flags.adc_overrun=0;
	BCF	_adc_flags, 1, B
	GOTO	_00163_DS_
_00159_DS_:
	BANKSEL	_adc_in
;	.line	147; adc.c	if(adc_in != adc_out) {
	MOVF	_adc_in, W, B
	BANKSEL	_adc_out
	XORWF	_adc_out, W, B
	BNZ	_00170_DS_
	GOTO	_00163_DS_
_00170_DS_:
;	.line	148; adc.c	ep2_num_bytes_to_send = adc_one_buf_size;
	MOVFF	_adc_one_buf_size, _ep2_num_bytes_to_send
	BANKSEL	(_ep2_num_bytes_to_send + 1)
	CLRF	(_ep2_num_bytes_to_send + 1), B
; ;multiply variable :_adc_out by variable _adc_one_buf_size and store in r0x00
	BANKSEL	_adc_one_buf_size
;	.line	149; adc.c	ep2_source_data = &(adc_buf[adc_one_buf_size*adc_out]);
	MOVF	_adc_one_buf_size, W, B
	BANKSEL	_adc_out
	MULWF	_adc_out, B
	MOVFF	PRODL, r0x00
	MOVLW	LOW(_adc_buf)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_buf)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVF	r0x01, W
	BANKSEL	(_ep2_source_data + 1)
	MOVWF	(_ep2_source_data + 1), B
	MOVF	r0x00, W
	BANKSEL	_ep2_source_data
	MOVWF	_ep2_source_data, B
	MOVLW	0x80
	BANKSEL	(_ep2_source_data + 2)
	MOVWF	(_ep2_source_data + 2), B
;	.line	150; adc.c	prepare_ep2_in();
	CALL	_prepare_ep2_in
	BANKSEL	_adc_out
;	.line	151; adc.c	adc_out++;
	INCF	_adc_out, F, B
	BANKSEL	_adc_nbufs
;	.line	152; adc.c	if(adc_out>=adc_nbufs) adc_out=0;
	MOVF	_adc_nbufs, W, B
	BANKSEL	_adc_out
	SUBWF	_adc_out, W, B
	BTFSS	STATUS, 0
	GOTO	_00163_DS_
	BANKSEL	_adc_out
	CLRF	_adc_out, B
_00163_DS_:
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_adc__adc_interrupt	code
_adc_interrupt:
;	.line	98; adc.c	void adc_interrupt(void) interrupt 1
	MOVFF	WREG, POSTDEC1
	MOVFF	STATUS, POSTDEC1
	MOVFF	BSR, POSTDEC1
	MOVFF	PRODL, POSTDEC1
	MOVFF	PRODH, POSTDEC1
	MOVFF	FSR0L, POSTDEC1
	MOVFF	FSR0H, POSTDEC1
	MOVFF	PCLATH, POSTDEC1
	MOVFF	PCLATU, POSTDEC1
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	BANKSEL	_adc_int_nr
;	.line	102; adc.c	adc_int_nr++;
	INCF	_adc_int_nr, F, B
	BTFSS	STATUS, 0
	BRA	_10154_DS_
	BANKSEL	(_adc_int_nr + 1)
	INCF	(_adc_int_nr + 1), F, B
_10154_DS_:
;	.line	104; adc.c	adc_buf[adc_buf_ptr++]=ADRESH; 
	MOVFF	_adc_buf_ptr, r0x00
	BANKSEL	_adc_buf_ptr
	INCF	_adc_buf_ptr, F, B
	MOVLW	LOW(_adc_buf)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_buf)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	_ADRESH, INDF0
;	.line	105; adc.c	adc_buf[adc_buf_ptr++]=ADRESL;
	MOVFF	_adc_buf_ptr, r0x00
	BANKSEL	_adc_buf_ptr
	INCF	_adc_buf_ptr, F, B
	MOVLW	LOW(_adc_buf)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_buf)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	_ADRESL, INDF0
;	.line	107; adc.c	PIR1bits.ADIF=0;
	BCF	_PIR1bits, 6
	BANKSEL	_adc_nchan
;	.line	109; adc.c	adc_nchan++;
	INCF	_adc_nchan, F, B
	BANKSEL	_adc_nchans
;	.line	110; adc.c	if (adc_nchan >= adc_nchans) {
	MOVF	_adc_nchans, W, B
	BANKSEL	_adc_nchan
	SUBWF	_adc_nchan, W, B
	BTFSS	STATUS, 0
	GOTO	_00140_DS_
	BANKSEL	_adc_nchan
;	.line	112; adc.c	adc_nchan=0;
	CLRF	_adc_nchan, B
	BANKSEL	_adc_in
;	.line	113; adc.c	adc_in++;
	INCF	_adc_in, F, B
	BANKSEL	_adc_nbufs
;	.line	114; adc.c	if(adc_in >= adc_nbufs) {
	MOVF	_adc_nbufs, W, B
	BANKSEL	_adc_in
	SUBWF	_adc_in, W, B
	BTFSS	STATUS, 0
	GOTO	_00136_DS_
	BANKSEL	_adc_in
;	.line	115; adc.c	adc_in = 0;
	CLRF	_adc_in, B
	BANKSEL	_adc_buf_ptr
;	.line	116; adc.c	adc_buf_ptr=0;
	CLRF	_adc_buf_ptr, B
_00136_DS_:
	BANKSEL	_adc_in
;	.line	118; adc.c	if(adc_in==adc_out) {
	MOVF	_adc_in, W, B
	BANKSEL	_adc_out
	XORWF	_adc_out, W, B
	BNZ	_00148_DS_
	GOTO	_00149_DS_
_00148_DS_:
	GOTO	_00138_DS_
_00149_DS_:
	BANKSEL	_adc_flags
;	.line	121; adc.c	adc_flags.adc_overrun=1;
	BSF	_adc_flags, 1, B
;	.line	122; adc.c	PIE1bits.ADIE=0;
	BCF	_PIE1bits, 6
;	.line	123; adc.c	ADCON0=0;
	CLRF	_ADCON0
;	.line	124; adc.c	return;
	GOTO	_00141_DS_
_00138_DS_:
;	.line	126; adc.c	adc_buf[adc_buf_ptr++]='D';
	MOVFF	_adc_buf_ptr, r0x00
	BANKSEL	_adc_buf_ptr
	INCF	_adc_buf_ptr, F, B
	MOVLW	LOW(_adc_buf)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_buf)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVLW	0x44
	MOVWF	INDF0
;	.line	127; adc.c	adc_buf[adc_buf_ptr++]=++adc_rec_nr;
	MOVFF	_adc_buf_ptr, r0x00
	BANKSEL	_adc_buf_ptr
	INCF	_adc_buf_ptr, F, B
	MOVLW	LOW(_adc_buf)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_buf)
	CLRF	r0x01
	ADDWFC	r0x01, F
	BANKSEL	_adc_rec_nr
	INCF	_adc_rec_nr, F, B
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	_adc_rec_nr, INDF0
_00140_DS_:
;	.line	130; adc.c	ADCON0 = (adc_cfg.chns[adc_nchan]<<2)+1;
	MOVFF	_adc_nchan, r0x00
	MOVLW	LOW(_adc_cfg + 3)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_cfg + 3)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	INDF0, r0x00
	RLCF	r0x00, W
	ANDLW	0x7e
	MOVWF	r0x01
	ADDWF	r0x01, F
	INCF	r0x01, W
	MOVWF	_ADCON0
_00141_DS_:
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	MOVFF	PREINC1, PCLATU
	MOVFF	PREINC1, PCLATH
	MOVFF	PREINC1, FSR0H
	MOVFF	PREINC1, FSR0L
	MOVFF	PREINC1, PRODH
	MOVFF	PREINC1, PRODL
	MOVFF	PREINC1, BSR
	MOVFF	PREINC1, STATUS
	MOVFF	PREINC1, WREG
	RETFIE	

; ; Starting pCode block
S_adc__adc_stop	code
_adc_stop:
;	.line	86; adc.c	void adc_stop(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
;	.line	91; adc.c	PIE1bits.ADIE=0;
	BCF	_PIE1bits, 6
;	.line	93; adc.c	ADCON0=0;
	CLRF	_ADCON0
	BANKSEL	_adc_flags
;	.line	94; adc.c	adc_flags.adc_sampling = 0;
	BCF	_adc_flags, 0, B
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_adc__adc_start	code
_adc_start:
;	.line	47; adc.c	void adc_start(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	BANKSEL	_adc_in
;	.line	50; adc.c	adc_in=0;
	CLRF	_adc_in, B
	BANKSEL	_adc_out
;	.line	51; adc.c	adc_out=0;
	CLRF	_adc_out, B
	BANKSEL	_adc_nchan
;	.line	52; adc.c	adc_nchan=0;
	CLRF	_adc_nchan, B
	BANKSEL	_adc_rec_nr
;	.line	53; adc.c	adc_rec_nr=0;
	CLRF	_adc_rec_nr, B
;	.line	54; adc.c	adc_int_nr=60;
	MOVLW	0x3c
	BANKSEL	_adc_int_nr
	MOVWF	_adc_int_nr, B
	BANKSEL	(_adc_int_nr + 1)
	CLRF	(_adc_int_nr + 1), B
	BANKSEL	_adc_buf_ptr
;	.line	55; adc.c	adc_buf_ptr=0;
	CLRF	_adc_buf_ptr, B
;	.line	56; adc.c	adc_buf[adc_buf_ptr++]='D';
	MOVFF	_adc_buf_ptr, r0x00
	BANKSEL	_adc_buf_ptr
	INCF	_adc_buf_ptr, F, B
	MOVLW	LOW(_adc_buf)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_buf)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVLW	0x44
	MOVWF	INDF0
;	.line	57; adc.c	adc_buf[adc_buf_ptr++]=adc_rec_nr;
	MOVFF	_adc_buf_ptr, r0x00
	BANKSEL	_adc_buf_ptr
	INCF	_adc_buf_ptr, F, B
	MOVLW	LOW(_adc_buf)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_buf)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVLW	0x00
	MOVWF	INDF0
	BANKSEL	_adc_flags
;	.line	58; adc.c	adc_flags.adc_overrun=0;
	BCF	_adc_flags, 1, B
;	.line	59; adc.c	TRISA=0xff;
	MOVLW	0xff
	MOVWF	_TRISA
;	.line	60; adc.c	ADCON1=0; // Configure A/D inputs
	CLRF	_ADCON1
;	.line	61; adc.c	PIR1bits.ADIF=0; // Clear A/D interrupt flag bit
	BCF	_PIR1bits, 6
;	.line	62; adc.c	PIE1bits.ADIE=1; // Enable A/D interrupts
	BSF	_PIE1bits, 6
;	.line	63; adc.c	IPR1bits.ADIP=1; //High Priority
	BSF	_IPR1bits, 6
;	.line	64; adc.c	ADCON0 = (adc_cfg.chns[adc_nchan]<<2)+1;
	MOVFF	_adc_nchan, r0x00
	MOVLW	LOW(_adc_cfg + 3)
	ADDWF	r0x00, F
	MOVLW	HIGH(_adc_cfg + 3)
	CLRF	r0x01
	ADDWFC	r0x01, F
	MOVFF	r0x00, FSR0L
	MOVFF	r0x01, FSR0H
	MOVFF	INDF0, r0x00
	RLCF	r0x00, W
	ANDLW	0x7e
	MOVWF	r0x01
	ADDWF	r0x01, F
	INCF	r0x01, W
	MOVWF	_ADCON0
;	.line	65; adc.c	ADCON2=0x06; //Left Justified, AN0 is analog, Vref comes from AVDD and AVSS
	MOVLW	0x06
	MOVWF	_ADCON2
;	.line	67; adc.c	CCPR2H=(adc_cfg.freq[1]);
	MOVFF	(_adc_cfg + 1), _CCPR2H
;	.line	68; adc.c	CCPR2L=(adc_cfg.freq[0]);
	MOVFF	_adc_cfg, _CCPR2L
;	.line	70; adc.c	CCP2CON = 0x0b;
	MOVLW	0x0b
	MOVWF	_CCP2CON
;	.line	72; adc.c	T3CON = 0; //Necessary to connect CCP2 to timer1
	CLRF	_T3CON
;	.line	73; adc.c	T1CON = 4;
	MOVLW	0x04
	MOVWF	_T1CON
;	.line	74; adc.c	TMR1L = 0;
	CLRF	_TMR1L
;	.line	75; adc.c	TMR1H = 0;
	CLRF	_TMR1H
;	.line	76; adc.c	T1CON = ((adc_cfg.freq[2]&3)<<4) | 1 ;
	MOVLW	0x03
	BANKSEL	(_adc_cfg + 2)
	ANDWF	(_adc_cfg + 2), W, B
	MOVWF	r0x00
	SWAPF	r0x00, W
	ANDLW	0xf0
	MOVWF	r0x01
	MOVLW	0x01
	IORWF	r0x01, W
	MOVWF	_T1CON
	BANKSEL	_adc_flags
;	.line	78; adc.c	adc_flags.adc_sampling=1;
	BSF	_adc_flags, 0, B
;	.line	80; adc.c	PIR1bits.ADIF=0; // Clear A/D interrupt flag bit
	BCF	_PIR1bits, 6
;	.line	81; adc.c	PIE1bits.ADIE=1;
	BSF	_PIE1bits, 6
;	.line	82; adc.c	INTCONbits.PEIE=1; //Enable peripheral interrupts
	BSF	_INTCONbits, 6
;	.line	83; adc.c	INTCONbits.GIE=1; //Enable all interrupts
	BSF	_INTCONbits, 7
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_adc__adc_reset	code
_adc_reset:
;	.line	41; adc.c	void adc_reset(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	BANKSEL	_adc_flags
;	.line	43; adc.c	adc_flags.adc_overrun = 0;
	BCF	_adc_flags, 1, B
	BANKSEL	_adc_flags
;	.line	44; adc.c	adc_flags.adc_sampling = 0;
	BCF	_adc_flags, 0, B
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block
S_adc__adc_init	code
_adc_init:
;	.line	25; adc.c	void adc_init()
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
	BANKSEL	_adc_nchans
;	.line	29; adc.c	adc_nchans=0;
	CLRF	_adc_nchans, B
;	.line	30; adc.c	for(i=0;i<8;i++) {
	CLRF	r0x00
	CLRF	r0x01
_00108_DS_:
	MOVF	r0x01, W
	ADDLW	0x80
	ADDLW	0x80
	BNZ	_00118_DS_
	MOVLW	0x08
	SUBWF	r0x00, W
_00118_DS_:
	BTFSC	STATUS, 0
	GOTO	_00111_DS_
;	.line	31; adc.c	if (adc_cfg.chns[i]<16) adc_nchans++;
	MOVLW	LOW(_adc_cfg + 3)
	ADDWF	r0x00, W
	MOVWF	r0x02
	MOVLW	HIGH(_adc_cfg + 3)
	ADDWFC	r0x01, W
	MOVWF	r0x03
	MOVFF	r0x02, FSR0L
	MOVFF	r0x03, FSR0H
	MOVFF	INDF0, r0x02
	MOVLW	0x10
	SUBWF	r0x02, W
	BTFSC	STATUS, 0
	GOTO	_00111_DS_
	BANKSEL	_adc_nchans
	INCF	_adc_nchans, F, B
;	.line	30; adc.c	for(i=0;i<8;i++) {
	INCF	r0x00, F
	BTFSC	STATUS, 0
	INCF	r0x01, F
	GOTO	_00108_DS_
; ;multiply lit val:0x02 by variable _adc_nchans and store in r0x00
; ;Unrolled 8 X 8 multiplication
; ;FIXME: the function does not support result==WREG
_00111_DS_:
;	.line	34; adc.c	adc_one_buf_size = 2*adc_nchans+2;
	BCF	STATUS, 0
	BANKSEL	_adc_nchans
	RLCF	_adc_nchans, W, B
	MOVWF	r0x00
	MOVLW	0x02
	ADDWF	r0x00, W
	BANKSEL	_adc_one_buf_size
	MOVWF	_adc_one_buf_size, B
;	.line	35; adc.c	adc_nbufs=ADC_BUF_SIZE/adc_one_buf_size;
	MOVFF	_adc_one_buf_size, POSTDEC1
	MOVLW	0xff
	MOVWF	POSTDEC1
	CALL	__divuchar
	BANKSEL	_adc_nbufs
	MOVWF	_adc_nbufs, B
	MOVF	PREINC1, W
	MOVF	PREINC1, W
	BANKSEL	_adc_in
;	.line	36; adc.c	adc_in=0;
	CLRF	_adc_in, B
	BANKSEL	_adc_out
;	.line	37; adc.c	adc_out=0;
	CLRF	_adc_out, B
	BANKSEL	_adc_nchan
;	.line	38; adc.c	adc_nchan=0;
	CLRF	_adc_nchan, B
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	

; ; Starting pCode block for Ival
	code
_err_ovr:
	DB	0x45, 0x3a, 0x4f, 0x56, 0x45, 0x52, 0x52, 0x55, 0x4e, 0x00


; Statistics:
; code size:	  980 (0x03d4) bytes ( 0.75%)
;           	  490 (0x01ea) words
; udata size:	  272 (0x0110) bytes (15.18%)
; access size:	    4 (0x0004) bytes


	end
