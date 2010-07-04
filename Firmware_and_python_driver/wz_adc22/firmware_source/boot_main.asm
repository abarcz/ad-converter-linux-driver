;--------------------------------------------------------
; File Created by SDCC : FreeWare ANSI-C Compiler
; Version 2.6.0 #4309 (Aug 16 2006)
; This file generated Fri Sep 15 11:04:56 2006
;--------------------------------------------------------
; PIC16 port for the Microchip 16-bit core micros
;--------------------------------------------------------
	list	p=18f4455
	__config 0x300000, 0xe1
	__config 0x300001, 0x3e
	__config 0x300002, 0xf8
	__config 0x300003, 0xe0
	__config 0x300005, 0xfc
	__config 0x300006, 0x9a
	__config 0x300008, 0xff
	__config 0x300009, 0xff
	__config 0x30000a, 0xfe
	__config 0x30000b, 0x9f
	__config 0x30000c, 0xff
	__config 0x30000d, 0xff

	radix dec

;--------------------------------------------------------
; public variables in this module
;--------------------------------------------------------
	global _stack
	global _stack_end
	global _init_boot
	global _main

;--------------------------------------------------------
; extern variables in this module
;--------------------------------------------------------
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
	extern _init_usb
	extern _dispatch_usb_event
	extern _adc_dispatch
	extern _adc_reset
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
	extern _application_data
;--------------------------------------------------------
;	Equates to used internal registers
;--------------------------------------------------------
STATUS	equ	0xfd8
PCL	equ	0xff9
PCLATH	equ	0xffa
PCLATU	equ	0xffb
INTCON	equ	0xff2
TOSL	equ	0xffd
TOSH	equ	0xffe
TOSU	equ	0xfff
TBLPTRL	equ	0xff6
TBLPTRH	equ	0xff7
TBLPTRU	equ	0xff8
TABLAT	equ	0xff5
FSR1L	equ	0xfe1
FSR2L	equ	0xfd9
POSTDEC1	equ	0xfe5
PREINC1	equ	0xfe4


; Internal registers
.registers	udata_ovr	0x0000
r0x00	res	1
r0x01	res	1
r0x02	res	1
r0x03	res	1


ustat_boot_main_00	udata	0X0200
_stack	res	254
_stack_end	res	1

;--------------------------------------------------------
; interrupt vector 
;--------------------------------------------------------

;--------------------------------------------------------
; global & static initialisations
;--------------------------------------------------------
; I code from now on!
; ; Starting pCode block
S_boot_main__main	code
_main:
;	.line	136; boot_main.c	init_boot();
	CALL	_init_boot
;	.line	138; boot_main.c	init_usb();
	CALL	_init_usb
;	.line	139; boot_main.c	adc_reset();
	CALL	_adc_reset
_00120_DS_:
;	.line	143; boot_main.c	adc_dispatch();
	CALL	_adc_dispatch
;	.line	144; boot_main.c	dispatch_usb_event();
	CALL	_dispatch_usb_event
;	.line	145; boot_main.c	if((application_data.invalid == 0) &&
	MOVLW	LOW(_application_data)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	MOVF	r0x00, W
	BTFSS	STATUS, 2
	GOTO	_00120_DS_
;	.line	146; boot_main.c	(GET_ACTIVE_CONFIGURATION() > FLASH_CONFIGURATION))
	MOVLW	0x03
	BANKSEL	_usb_active_cfg
	SUBWF	_usb_active_cfg, W, B
	BTFSS	STATUS, 0
	GOTO	_00120_DS_
;	.line	148; boot_main.c	application_data.main();
	MOVLW	LOW(_application_data + 22)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 22)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 22)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	TBLRD*+	
	MOVFF	TABLAT, r0x01
	TBLRD*+	
	MOVFF	TABLAT, r0x02
	MOVFF	INTCON, POSTDEC1
	BCF	INTCON, 7
	PUSH	
	MOVLW	LOW(_00128_DS_)
	MOVWF	TOSL
	MOVLW	HIGH(_00128_DS_)
	MOVWF	TOSH
	MOVLW	UPPER(_00128_DS_)
	MOVWF	TOSU
	MOVF	PREINC1, W
	MOVWF	INTCON
	MOVFF	r0x02, PCLATU
	MOVFF	r0x01, PCLATH
	MOVF	r0x00, W
	MOVWF	PCL
_00128_DS_:
;	.line	149; boot_main.c	INTCON = 0; // Forbid interrupts
	CLRF	_INTCON
	GOTO	_00120_DS_
	RETURN	

; ; Starting pCode block
S_boot_main__init_boot	code
_init_boot:
;	.line	90; boot_main.c	void init_boot(void)
	MOVFF	FSR2L, POSTDEC1
	MOVFF	FSR1L, FSR2L
	MOVFF	r0x00, POSTDEC1
	MOVFF	r0x01, POSTDEC1
	MOVFF	r0x02, POSTDEC1
	MOVFF	r0x03, POSTDEC1
;	.line	94; boot_main.c	ADCON1 = 0x0F;
	MOVLW	0x0f
	MOVWF	_ADCON1
;	.line	95; boot_main.c	CMCON  = 0x07;
	MOVLW	0x07
	MOVWF	_CMCON
;	.line	96; boot_main.c	TRISA  = 0xFE;
	MOVLW	0xfe
	MOVWF	_TRISA
;	.line	97; boot_main.c	PORTA  = 0x01;
	MOVLW	0x01
	MOVWF	_PORTA
;	.line	100; boot_main.c	while(count)
	CLRF	r0x00
	CLRF	r0x01
	MOVLW	0x08
	MOVWF	r0x02
	CLRF	r0x03
_00105_DS_:
	MOVF	r0x00, W
	IORWF	r0x01, W
	IORWF	r0x02, W
	IORWF	r0x03, W
	BTFSC	STATUS, 2
	GOTO	_00107_DS_
;	.line	102; boot_main.c	count--;
	MOVLW	0xff
	ADDWF	r0x00, F
	MOVLW	0xff
	ADDWFC	r0x01, F
	MOVLW	0xff
	ADDWFC	r0x02, F
	MOVLW	0xff
	ADDWFC	r0x03, F
	GOTO	_00105_DS_
_00107_DS_:
;	.line	105; boot_main.c	PORTA  = 0x00;
	CLRF	_PORTA
;	.line	108; boot_main.c	if(application_data.invalid == 0) 
	MOVLW	LOW(_application_data)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, r0x00
	MOVF	r0x00, W
	BTFSS	STATUS, 2
	GOTO	_00109_DS_
;	.line	111; boot_main.c	device_descriptor        = application_data.device_descriptor;
	MOVLW	LOW(_application_data + 1)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 1)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 1)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _device_descriptor
	TBLRD*+	
	MOVFF	TABLAT, (_device_descriptor + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_device_descriptor + 2)
;	.line	112; boot_main.c	configuration_descriptor = application_data.configuration_descriptor;
	MOVLW	LOW(_application_data + 4)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 4)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 4)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _configuration_descriptor
	TBLRD*+	
	MOVFF	TABLAT, (_configuration_descriptor + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_configuration_descriptor + 2)
;	.line	113; boot_main.c	string_descriptor        = application_data.string_descriptor;
	MOVLW	LOW(_application_data + 7)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 7)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 7)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _string_descriptor
	TBLRD*+	
	MOVFF	TABLAT, (_string_descriptor + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_string_descriptor + 2)
;	.line	114; boot_main.c	ep_init                  = application_data.ep_init;
	MOVLW	LOW(_application_data + 10)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 10)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 10)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _ep_init
	TBLRD*+	
	MOVFF	TABLAT, (_ep_init + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_ep_init + 2)
;	.line	115; boot_main.c	ep_in                    = application_data.ep_in;
	MOVLW	LOW(_application_data + 13)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 13)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 13)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _ep_in
	TBLRD*+	
	MOVFF	TABLAT, (_ep_in + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_ep_in + 2)
;	.line	116; boot_main.c	ep_out                   = application_data.ep_out;
	MOVLW	LOW(_application_data + 16)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 16)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 16)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _ep_out
	TBLRD*+	
	MOVFF	TABLAT, (_ep_out + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_ep_out + 2)
;	.line	117; boot_main.c	ep_setup                 = application_data.ep_setup;
	MOVLW	LOW(_application_data + 19)
	MOVWF	TBLPTRL
	MOVLW	HIGH(_application_data + 19)
	MOVWF	TBLPTRH
	MOVLW	UPPER(_application_data + 19)
	MOVWF	TBLPTRU
	TBLRD*+	
	MOVFF	TABLAT, _ep_setup
	TBLRD*+	
	MOVFF	TABLAT, (_ep_setup + 1)
	TBLRD*+	
	MOVFF	TABLAT, (_ep_setup + 2)
	GOTO	_00111_DS_
_00109_DS_:
;	.line	122; boot_main.c	device_descriptor        = &boot_device_descriptor;
	MOVLW	LOW(_boot_device_descriptor)
	BANKSEL	_device_descriptor
	MOVWF	_device_descriptor, B
	MOVLW	HIGH(_boot_device_descriptor)
	BANKSEL	(_device_descriptor + 1)
	MOVWF	(_device_descriptor + 1), B
	MOVLW	UPPER(_boot_device_descriptor)
	BANKSEL	(_device_descriptor + 2)
	MOVWF	(_device_descriptor + 2), B
;	.line	123; boot_main.c	configuration_descriptor = boot_configuration_descriptor;
	MOVLW	LOW(_boot_configuration_descriptor)
	BANKSEL	_configuration_descriptor
	MOVWF	_configuration_descriptor, B
	MOVLW	HIGH(_boot_configuration_descriptor)
	BANKSEL	(_configuration_descriptor + 1)
	MOVWF	(_configuration_descriptor + 1), B
	MOVLW	UPPER(_boot_configuration_descriptor)
	BANKSEL	(_configuration_descriptor + 2)
	MOVWF	(_configuration_descriptor + 2), B
;	.line	124; boot_main.c	string_descriptor        = boot_string_descriptor;
	MOVLW	LOW(_boot_string_descriptor)
	BANKSEL	_string_descriptor
	MOVWF	_string_descriptor, B
	MOVLW	HIGH(_boot_string_descriptor)
	BANKSEL	(_string_descriptor + 1)
	MOVWF	(_string_descriptor + 1), B
	MOVLW	UPPER(_boot_string_descriptor)
	BANKSEL	(_string_descriptor + 2)
	MOVWF	(_string_descriptor + 2), B
;	.line	125; boot_main.c	ep_init                  = boot_ep_init;
	MOVLW	LOW(_boot_ep_init)
	BANKSEL	_ep_init
	MOVWF	_ep_init, B
	MOVLW	HIGH(_boot_ep_init)
	BANKSEL	(_ep_init + 1)
	MOVWF	(_ep_init + 1), B
	MOVLW	UPPER(_boot_ep_init)
	BANKSEL	(_ep_init + 2)
	MOVWF	(_ep_init + 2), B
;	.line	126; boot_main.c	ep_in                    = &boot_ep_in;
	MOVLW	LOW(_boot_ep_in)
	BANKSEL	_ep_in
	MOVWF	_ep_in, B
	MOVLW	HIGH(_boot_ep_in)
	BANKSEL	(_ep_in + 1)
	MOVWF	(_ep_in + 1), B
	MOVLW	UPPER(_boot_ep_in)
	BANKSEL	(_ep_in + 2)
	MOVWF	(_ep_in + 2), B
;	.line	127; boot_main.c	ep_out                   = &boot_ep_out;
	MOVLW	LOW(_boot_ep_out)
	BANKSEL	_ep_out
	MOVWF	_ep_out, B
	MOVLW	HIGH(_boot_ep_out)
	BANKSEL	(_ep_out + 1)
	MOVWF	(_ep_out + 1), B
	MOVLW	UPPER(_boot_ep_out)
	BANKSEL	(_ep_out + 2)
	MOVWF	(_ep_out + 2), B
;	.line	128; boot_main.c	ep_setup                 = &boot_ep_setup;
	MOVLW	LOW(_boot_ep_setup)
	BANKSEL	_ep_setup
	MOVWF	_ep_setup, B
	MOVLW	HIGH(_boot_ep_setup)
	BANKSEL	(_ep_setup + 1)
	MOVWF	(_ep_setup + 1), B
	MOVLW	UPPER(_boot_ep_setup)
	BANKSEL	(_ep_setup + 2)
	MOVWF	(_ep_setup + 2), B
_00111_DS_:
	MOVFF	PREINC1, r0x03
	MOVFF	PREINC1, r0x02
	MOVFF	PREINC1, r0x01
	MOVFF	PREINC1, r0x00
	MOVFF	PREINC1, FSR2L
	RETURN	



; Statistics:
; code size:	  606 (0x025e) bytes ( 0.46%)
;           	  303 (0x012f) words
; udata size:	  255 (0x00ff) bytes (14.23%)
; access size:	    4 (0x0004) bytes


	end
