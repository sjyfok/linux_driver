ARCH=arm

CROSS_COMPILE=arm-linux-

obj-m = sjzc_gpi.o

CURRENT_PATH:=$(shell pwd)

LINUX_KERNEL_PATH:=/home/song/imx/kernel-imx/kernel-imx

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
app:
	#$(CROSS_COMPILE)gcc -o sjzc_gpi.o sjzc_gpi.c
	$(CROSS_COMPILE)gcc -o $*.o $<
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean

