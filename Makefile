obj-m	:= patch_guard.o
patch_guard-y := main.o persistency.o file.o hash.o

KERNELDIR ?= /usr/src/linux-headers-5.4.0-48-generic
PWD       := $(shell pwd)
 
all: debug
 
release:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

ccflags-y := -g -Og -O0
debug:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions debug
