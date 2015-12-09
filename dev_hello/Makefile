ARCH=arm

CROSS_COMPILE=arm-linux-

obj-m = dev_hello.o

CURRENT_PATH:=$(shell pwd)

LINUX_KERNEL_PATH:=/home/song/imx/kernel-imx/kernel-imx

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
app:
	$(CROSS_COMPILE)gcc -o dev_hello.o dev_hello.c
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean

