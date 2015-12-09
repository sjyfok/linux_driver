ARCH=arm

CROSS_COMPILE=arm-linux-

LOCAL_NAME=globalmem
LOCAL_OBJ=$(LOCAL_NAME).o
LOCAL_SRC=$(LOCAL_NAME).c


CURRENT_PATH:=$(shell pwd)

LINUX_KERNEL_PATH:=/home/song/imx/kernel-imx/kernel-imx

obj-m += $(LOCAL_OBJ)

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules
app:
	$(CROSS_COMPILE)gcc -o $(LOCAL_OBJ) $(LOCAL_SRC)
	#$(CROSS_COMPILE)gcc -o globalmem.o globalmem.c
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean


