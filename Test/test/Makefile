ifneq ($(KERNELRELEASE),)
    obj-m	:= ster_test.o    
else
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
endif
