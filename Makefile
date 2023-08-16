TARGET_MODULE:=xsmi
ARCH:=arm64

obj-m 	:= $(TARGET_MODULE).o 
 
KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean
