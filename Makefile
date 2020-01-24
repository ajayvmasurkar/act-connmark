ifneq ($(KERNELRELEASE),)
# kbuild part of makefile
ifndef CONFIG_NET_ACT_CONNMARK
$(error ** You need to enable NET_ACT_CONNMARK in your kernel **)
endif

obj-m := act_connmark.o
else

# Normal Makefile

KERNELDIR := /lib/modules/`uname -r`/build
all: 
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules

debug:
	$(MAKE) -C $(KERNELDIR) EXTRA_CFLAGS=-DDEBUG M=`pwd` modules

modules_install:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules_install

clean:
	rm -rf *.o *.ko *.mod.c .*.cmd Module.symvers modules.order .tmp_versions

endif
