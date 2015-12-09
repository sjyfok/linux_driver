/*
 * =====================================================================================
 *
 *	Filename:  am335x_gpo_driver.c
 *
 * 	Description: gpo driver in the kernel of linux-3.2.0 on am335x plat 
 *
 *	Version:  1.0.1
 *	Created:  
 *	Revision:  
 *	Compiler:  
 *
 *	Author:  GQM
 *	Company: HNLX 
 *
 * =====================================================================================
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/leds.h>
#include <linux/io.h>

#include <linux/semaphore.h>
#include <linux/kernel.h>
#include <linux/cdev.h>

#include <linux/types.h>
#include <linux/fs.h>
#include <mach/gpio.h>

#include <linux/gpio.h>
//
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
//
#define GPO_NUMBER	2
//
//#define DEVICE_NAME		"sjzc_gpo"
#define DEVICE_NAME		"eoas_leds"
#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))
//
#define ON		0
#define OFF		1

//注册设备
struct gpo_dev
{
	struct cdev cdev;
	unsigned char value;
};

//指定GPO引脚
static unsigned long gpo_table [] =
{	
	IMX_GPIO_NR(3,30),//LED-A2
	IMX_GPIO_NR(3,31),//LED-B1
//	GPIO_TO_PIN(3,19),//LED-B2
//	GPIO_TO_PIN(3,21),//LED-C1
//	GPIO_TO_PIN(3,15),//LED-C2
//	GPIO_TO_PIN(2,24),//LED-D1
//	GPIO_TO_PIN(2,22),//LED-D2
};

static int gpo_open(struct inode *inode, struct file *file)
{
	//
	printk("%s driver opened!\n", DEVICE_NAME);
	//
	return 0;
}

//static int gpo_ioctl(struct inode *inode, struct file *file,
//		unsigned int cmd, unsigned long arg)
static int gpo_ioctl(struct file *file,
		unsigned int cmd, unsigned long arg)
{
	//
	//printk("ioctl start!\n");
	//printk("arg = %d\n", arg);
	if (arg > GPO_NUMBER)
	{
		return -EINVAL;
	}
	//printk("cmd = %d\n", cmd);
	switch(cmd)
	{
	case ON:
		gpio_set_value(gpo_table[arg], 0);
		return 0;
	case OFF:
		gpio_set_value(gpo_table[arg], 1);
		return 0;
		
	default:
		return -EINVAL;
	}
}

static int gpo_release(struct inode *inode, struct file *file)
{
	printk("%s released!\n", DEVICE_NAME);
	return 0;
}

static struct file_operations s3c6410_gpo_fops = 
{
	.owner	=	THIS_MODULE,
	.open	=	gpo_open,
	.unlocked_ioctl	=	gpo_ioctl,
	.release =  gpo_release,
};

static struct miscdevice gpo_miscdev =
{
	 .minor	= MISC_DYNAMIC_MINOR,
	 .name	= DEVICE_NAME,
	 .fops	= &s3c6410_gpo_fops
};

static int gpo_probe(struct platform_device *dev)
{
	int ret = 0;
	int i = 0;
	//
	int result = 0;
	
	for (i = 0; i < GPO_NUMBER; i++)
	{
		result = gpio_request(gpo_table[i], "Leds");//usr1
		if (result != 0)
		{
			printk("gpio request %s failed!\n", gpo_table[i]);
		}
		result = gpio_direction_output(gpo_table[i], 0);//0:reset high 1:reset low
		if (result != 0)
		{
			printk("gpio direction %s failed!\n", gpo_table[i]);
		}
		//
		gpio_set_value(gpo_table[i], 1);//all high level
	}
	
	//
	printk("probing %s\n", DEVICE_NAME);
	//
	ret = misc_register(&gpo_miscdev);
	if (ret)
	{
		printk("Failed to register miscdev for "DEVICE_NAME".\n");
	}
	return ret;
}

static int gpo_remove(struct platform_device *dev)
{
	int i = 0;
	//
	misc_deregister(&gpo_miscdev);
	//
	for (i = 0; i < GPO_NUMBER; i++)
	{
		gpio_free(gpo_table[i]);
	}
	printk("\n%s removed!\n", DEVICE_NAME);
	//
	return 0;
}

struct platform_device *gpo_device;

static struct platform_driver gpo_driver = {
    .driver = {
        .name    = DEVICE_NAME,
        .owner   = THIS_MODULE,
    },
    .probe   = gpo_probe,
    .remove  = gpo_remove,
};




static int __init gpo_init(void)
{
	int rc;
	//
	printk("\n%s init......\n", DEVICE_NAME);
	gpo_device = platform_device_alloc(DEVICE_NAME, -1);
    if (!gpo_device)
	{
        return -ENOMEM;
	}
	//
	rc = platform_device_add(gpo_device);
    if (rc < 0) 
	{
        platform_device_put(gpo_device);
        return rc;
    }
	//
	rc = platform_driver_register(&gpo_driver);
    if (rc < 0)
	{
        platform_driver_unregister(gpo_device);
	}
	//
    return rc;	
}

static void __exit gpo_exit(void)
{
	//
	platform_driver_unregister(&gpo_driver);
    platform_device_unregister(gpo_device);
	//
    printk("%s exit!\n", DEVICE_NAME);
}


module_init(gpo_init);
module_exit(gpo_exit);

MODULE_AUTHOR("gqm gongqingmin@hnlx.com.cn");
MODULE_DESCRIPTION("am335x gpo driver V1.0.1");
MODULE_LICENSE("GPL");


