ARCH=arm

CROSS_COMPILE=arm-linux-

obj-m = sjzc_gpo.o

CURRENT_PATH:=$(shell pwd)

LINUX_KERNEL_PATH:=/home/song/imx/kernel-imx/kernel-imx

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
app:
	$(CROSS_COMPILE)gcc -o sjzc_gpo.o sjzc_gpo.c
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean

