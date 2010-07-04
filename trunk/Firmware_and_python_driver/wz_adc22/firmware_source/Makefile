
# $Id: Makefile,v 1.2 2006/03/05 19:39:31 gaufille Exp $

PIC_TYPE=PIC18F4550
sdcc_PIC_TYPE=18f4455
#TOOLSDIR=$(shell dirname `pwd`)/tools
TOOLSDIR=/usr/
CC=$(TOOLSDIR)/bin/sdcc
ASM=$(TOOLSDIR)/bin/gpasm
GLNK=$(TOOLSDIR)/bin/gplink
LNK=$(TOOLSDIR)/bin/sdcc
ODYSSEY=$(TOOLSDIR)/bin/odyssey
CRT0=$(TOOLSDIR)/share/sdcc/lib/pic16/crt0.o
LIBIO=$(TOOLSDIR)/share/sdcc/lib/pic16/libio18f4455.lib
LIBC18F=$(TOOLSDIR)/share/sdcc/lib/pic16/libc18f.lib
#DEBUG=-D_DEBUG
DEBUG=
#DEBUG=--optimize-df --opt-code-size --optimize-cmp --obanksel=2
#DEBUG=--ivt-loc=0x2000 --fstack --opt-code-size --obanksel=2 --optimize-df --optimize-cmp
all: boot.hex


# Link with sdcc causes an error when accessing UCFG and UCON : access bit is set
# but it should not. Using the CVS version 20050112 of gplink to link the code
# solves this issue
boot.hex: boot_main.o adc.o usb.o usb_descriptors.o ep0.o ep1.o ep2.o application.o flash.o
	$(GLNK) -w -s 18f4550.lkr -m $+ $(TOOLSDIR)/share/sdcc/lib/pic16/pic$(sdcc_PIC_TYPE).lib \
	       $(TOOLSDIR)/share/sdcc/lib/pic16/libsdcc.lib $(CRT0) $(LIBIO) $(LIBC18F) -o $@

%.o: %.c
	$(CC) -mpic16 $(DEBUG) -p$(sdcc_PIC_TYPE) -Iinclude -c $< -o $@

clean:
	rm -f *.o *.rel *.lst *.cod *.hex *.map *.asm

prog: all
	(echo "Blanking...";\
	$(ODYSSEY) $(PIC_TYPE) erase) && \
	(echo "Writing...";\
	$(ODYSSEY) $(PIC_TYPE) write boot.hex)

.PHONY: all clean prog erase

application.o adc.o boot_main.o usb.o usb_descriptors.o ep0.o ep1.o ep2.o flash.o: usb.h usb_descriptors.h ep0.h ep1.h ep2.h application.h usb_std_req.h common_types.h debug.h flash.h Makefile
