/*
 * Driver for GPIO lines capable of generating interrupts.
 *
 * Copyright 2015-11-19 GQM
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
//
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
//#include <linux/semaphore.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/irq.h>
#include <linux/interrupt.h> //IRQ_TYPE_EDGE_FALLING
#include <linux/gpio.h>
#include <linux/miscdevice.h>
#include <linux/of_platform.h>
#include <linux/delay.h>//udelay
//
#include <asm/uaccess.h> //copy_to_user
#include <linux/wait.h>
#include <linux/sched.h>
//  
#include <mach/gpio.h>
#include <mach/irqs.h>
//#include <plat/mux.h>
#include <linux/platform_device.h>
// 
#define GPI_NUMBER				1  
#define DEVICE_NAME        		"eoas_gpio"
//#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

//
MODULE_LICENSE("GPL"); 
//added by helun
//extern int powerdown;

//
static wait_queue_head_t		wait_interrupt;
static int interrupt_flag = 0;
//
static unsigned long gpi_table[] =
{	
	//
	//GPIO_TO_PIN(3, 14),
	IMX_GPIO_NR(3, 16),
};

//
static int check_num = 3; //check the power pin for 3 times
//
module_param(check_num, uint, 0755);
MODULE_PARM_DESC(check_num, "check times");

//
static int gpi_open(struct inode *inode, struct file *file)
{
	//
	printk("%s driver opened!\n", DEVICE_NAME);
	//
	return 0;
}

static int gpi_release(struct inode *inode, struct file *file)
{
	printk("%s released!\n", DEVICE_NAME);
	return 0;
}

static ssize_t gpi_read(struct file *flie, char __user *buf, size_t count)
{
    int regval = 0;
	unsigned char key_val[1]={1}; 
	//
	if(count < 1)
	{
		return - ERESTARTSYS;
	}
	//
	if (wait_event_interruptible(wait_interrupt, interrupt_flag != 0))
	{
		return - ERESTARTSYS;
	}
	copy_to_user(buf, &key_val[0], sizeof(key_val)); 
	//
	interrupt_flag = 0;   
	//         
    return sizeof(key_val); 
}

//sjzc gpi interrupt handler.
static irqreturn_t gpi_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	int i=0;
	unsigned char l_flag=0;
	int powerdown = 0;
	//
	if(0 == gpio_get_value(gpi_table[0]))
	{
		for(i=0; i<check_num; i++)
		{
			udelay(1);
			if(0 == gpio_get_value(gpi_table[0]))
			{
				l_flag++;
			}
		}
		//
		if(l_flag >= check_num)
		{
			printk("Low power !\n");
			interrupt_flag = 1;
			//added by helun
			powerdown = 1;
			wake_up_interruptible(&wait_interrupt);
		}
		//
	}
	else if(1 == gpio_get_value(gpi_table[0]))
	{
		for(i=0; i<check_num; i++)
		{
			udelay(1);
			if(1 == gpio_get_value(gpi_table[0]))
			{
				l_flag++;
			}
		}
		//
		if(l_flag >= check_num)
		{
			printk("Power on !\n");
			interrupt_flag = 1;
			powerdown = 0;
			wake_up_interruptible(&wait_interrupt);
		}
	}
	//return IRQ_HANDLED;
	return powerdown;
}


static const struct file_operations gpi_fops = {
	.owner   = THIS_MODULE,
	.open    = gpi_open,
	.read    = gpi_read,
	.release = gpi_release,
};

static struct miscdevice gpi_miscdev =
{
	 .minor	= MISC_DYNAMIC_MINOR,
	 .name	= DEVICE_NAME,
	 .fops	= &gpi_fops
};

static int gpi_probe(struct platform_device *dev)
{
	int ret = 0;
	int i = 0;
	int irq =0;
	//
	int result = 0;
	
	for (i = 0; i < GPI_NUMBER; i++)
	{
		result = gpio_request(gpi_table[i], "key");//usr1
		if (result != 0)
		{
			printk("gpio request %s failed!\n", gpi_table[i]);
		}
		result = gpio_direction_input(gpi_table[i]);//set to input
		if (result != 0)
		{
			printk("gpio direction %s failed!\n", gpi_table[i]);
		}
		//
		irq = gpio_to_irq(gpi_table[i]);  
		if(irq < 0)
		{  
		    printk("gpio_to_irq() failed key_inter!\n");  
		    ret = irq; 
			gpio_free(gpi_table[i]); 
		    return ret;   
		}  
		printk("irq = %d\n", irq); 
		//
		irq_set_irq_type(irq, IRQ_TYPE_EDGE_FALLING);
		//irq_set_irq_type(irq, IRQF_TRIGGER_LOW);
		//irq_set_irq_type(irq, IRQ_LEVEL);//IRQ_TYPE_LEVEL_LOW
		//ret = request_irq(irq, gpi_interrupt, IRQ_TYPE_EDGE_FALLING | IRQF_DISABLED, "key", NULL);
		ret = request_irq(irq, gpi_interrupt, IRQ_TYPE_EDGE_BOTH | IRQF_DISABLED, "key", NULL);//
		//ret = request_irq(irq, gpi_interrupt, IRQF_TRIGGER_LOW | IRQF_DISABLED, "key", NULL);
		//ret = request_irq(irq, gpi_interrupt, IRQ_TYPE_LEVEL_LOW, "key", NULL);
		if(ret)
		{  
		    printk("request_irq() key_inter failed ! %d\n", ret);  
		    return ret;  
		}  
		//
	}
	//
	printk("probing %s\n", DEVICE_NAME);
	//
	ret = misc_register(&gpi_miscdev);
	if (ret)
	{
		printk("Failed to register miscdev for %s.\n", DEVICE_NAME);
	}
	return ret;
}

static int gpi_remove(struct platform_device *dev)
{
	//
	misc_deregister(&gpi_miscdev);
	printk("\n%s removed!\n", DEVICE_NAME);
	
	return 0;
}

struct platform_device *gpi_device;

static struct platform_driver gpi_driver = {
    .driver = {
        .name    = DEVICE_NAME,
        .owner   = THIS_MODULE,
    },
    .probe   = gpi_probe,
    .remove  = gpi_remove,
};


static int __init gpi_init(void)
{
	int rc;
	//
	//
	printk("\n%s init......\n", DEVICE_NAME);
	gpi_device = platform_device_alloc(DEVICE_NAME, -1);
    if (!gpi_device)
        return -ENOMEM;
	//
	rc = platform_device_add(gpi_device);
    if (rc < 0) {
        platform_device_put(gpi_device);
        return rc;
    }
	//
	rc = platform_driver_register(&gpi_driver);
    if (rc < 0)
        platform_driver_unregister(&gpi_driver);
	//
	init_waitqueue_head(&wait_interrupt);
	//
	printk(KERN_ALERT "check_num %d.\n", check_num);
	//
    return rc;	
}

static void __exit gpi_exit(void)
{
	int i;
	int irq = 0;
	//
	for(i=0;i<GPI_NUMBER;i++)
	{
		irq = gpio_to_irq(gpi_table[i]);  	 
		free_irq(irq, NULL);
		gpio_free(gpi_table[i]);
	}
	//
	platform_driver_unregister(&gpi_driver);
    platform_device_unregister(gpi_device);
	//
    printk("%s exit!\n", DEVICE_NAME);
	interrupt_flag = 0;
}

module_init(gpi_init);
module_exit(gpi_exit);

//MODULE_LICENSE(" ");
MODULE_AUTHOR("GQM <gongqingmin@hnlx.com.cn>");
MODULE_VERSION("sjzc gpi Driver V1.0.2");
MODULE_DESCRIPTION("Added power on.");
  


