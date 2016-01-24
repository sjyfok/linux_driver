/* linux/drivers/char/watchdog/s3c2410_wdt.c
 *
 * Copyright (c) 2004 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * S3C2410 Watchdog Timer Support
 *
 * Based on, softdog.c by Alan Cox,
 *     (c) Copyright 1996 Alan Cox <alan@lxorguk.ukuu.org.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
//
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/io.h>

#include <mach/map.h>

#include <plat/regs-timer.h>
//
#include "s3c6410_timer_driver.h"


//
static void __iomem	*timer_base;
static struct clk	*timer_clock;
//




unsigned int interrup_num;

/* interrupt handler code */
static irqreturn_t s3c6410timer_irq(int irqno, void *param)
{
	unsigned long uTemp1;

	//
	if(irqno == TIMER_IRQ)
	{
		uTemp1 = readl(S3C_TINT_CSTAT);
		//uTemp1 = (uTemp1 &(~(0x41)))|(1<<6);
		uTemp1 |= (1<<6);
		writel(uTemp1, S3C_TINT_CSTAT); 
		//


		interrup_num++;
		if(interrup_num >= 11111)//90US
		{
			interrup_num = 0;
			printk("1 S!\n");
		}

		//printk("S3C6410 Timer Interrupted!\n");
	}
	else
	{
		printk("Other irqs!\n");
	}
	//
	return IRQ_HANDLED;
}
//

static void s3c6410timer_stop(void)
{
	unsigned long uTemp0;
	//
	//-----------------------Set SFRs--------------------------
	uTemp0 = readl(S3C_TCON);
	uTemp0 = uTemp0 & (0xfffffeff);
	writel(uTemp0, S3C_TCON);// Timer1 stop
	//
	//
}
//

static void s3c6410timer_start(void)
{
	unsigned long uTemp0;
	unsigned long uTemp1;
	unsigned long uTemp2;
	unsigned long uTemp3;

	unsigned long pclk;
	unsigned long tcnt;
	//
	//timer_clock = clk_get(NULL, "timers");
	timer_clock = clk_get(NULL, "pclk");
	if (!timer_clock) 
	{
		printk(KERN_ERR "failed to get timer clock source\n");
		return -ENOENT;
	}
	pclk = clk_get_rate(timer_clock);
	printk("pclk %d \n", pclk);
	//
	tcnt = 90; //90US
	//clk_enable(timer_clock);
	//
	//-----------------------Set SFRs--------------------------
	uTemp0 = __raw_readl(S3C_TCON);
	uTemp0 = uTemp0 & (0xfffffeff);
	__raw_writel(uTemp0, S3C_TCON);// Timer1 stop
	//
	uTemp2 = __raw_readl(S3C_TCFG0);
	//uTemp2 = (uTemp2 & (~(0xffff00)))|((uPrescaler-1)<<8)|(uDzlen<<16);
	uTemp2 &= ~S3C_TCFG_PRESCALER0_MASK;
	uTemp2 |= ((uPrescaler-1)<<0);
	__raw_writel(uTemp2, S3C_TCFG0);
	//
	uTemp1 = __raw_readl(S3C_TCFG1);
	uTemp1 &= ~S3C_TCFG1_MUX0_MASK;
	uTemp1 |= S3C2410_TCFG1_MUX1_DIV2; //0 divide
	__raw_writel(uTemp1, S3C_TCFG1);
	//
	__raw_writel(tcnt, S3C_TCNTB(1));
	//__raw_writel(0, S3C_TCMPB(1));
	//
	uTemp1 = __raw_readl(S3C_TINT_CSTAT);
	//uTemp1 = (uTemp1 &(~(0x41)))|(1<<0)|(1<<6);
	uTemp1 |= 0x1<<1;
	__raw_writel(uTemp1, S3C_TINT_CSTAT); 
	//
/*
	uTemp3 = __raw_readl(S3C_TCON);
	uTemp3 &= ~0xf1<<8;
	uTemp3 |= 0xb<<8;//disable deadzone,auto-reload,inv-off,update TCNTB0&TCMPB0,start timer 1
	__raw_writel(uTemp3, S3C_TCON);// timer1 start
	//
	uTemp3 &= ~0x2<<8;//clear manual update bit
	__raw_writel(uTemp3, S3C_TCON);
*/
	//
	uTemp3 = __raw_readl(S3C_TCON);
	uTemp3 = (uTemp3 &(~(0x00f00)))|(uAutoreload<<11)|(uEnInverter<<10)|(1<<9)|(1<<8);
	__raw_writel(uTemp3, S3C_TCON); // timer1 manual update

	//
	uTemp3 = (uTemp3 &(~(0x00f00)))|(uAutoreload<<11)|(uEnInverter<<10)|(0<<9)|(1<<8);
	//uTemp3 = uTemp3 & (~(0x2)<<8);
	__raw_writel(uTemp3, S3C_TCON);// timer1 start
	//
	printk("timer1 init success!\n");
	//
	
}
#if 0
{
	unsigned long uTemp0;
	unsigned long uTemp1;
	unsigned long uTemp2;
	unsigned long uTemp3;

	unsigned long pclk;
	unsigned long tcnt;
	//
	uTemp0 = __raw_readl(timer_base+rTCON);
	uTemp1 = __raw_readl(timer_base+rTCFG1);
	uTemp2 = __raw_readl(timer_base+rTCFG0);
	//
	
	//-----------------------Set SFRs--------------------------
	//uTemp0 = __raw_readl(timer_base+rTCON);
	uTemp0 = uTemp0 & (0xfffffeff);
	__raw_writel(uTemp0, timer_base+rTCON);// Timer1 stop
	//
	uTemp2 = __raw_readl(timer_base+rTCFG0);
	//uTemp2 = (uTemp2 & (~(0xffff00)))|((uPrescaler-1)<<8)|(uDzlen<<16);
	uTemp2 = (uTemp2 & (~(0x000000ff)))|((uPrescaler-1)<<0);
	__raw_writel(uTemp2, timer_base+rTCFG0);
	//
	uTemp1 = __raw_readl(timer_base+rTCFG1);
	uTemp1 = (uTemp1 & (~(0x000000f0)))|(0<<4); //0 divide
	__raw_writel(uTemp1, timer_base+rTCFG1);
	//
	//timer_clock = clk_get(NULL, "timers");
	timer_clock = clk_get(NULL, "pclk");
	if (!timer_clock) 
	{
		printk(KERN_ERR "failed to get timer clock source\n");
		return -ENOENT;
	}
	pclk = clk_get_rate(timer_clock);
	printk("pclk %d \n", pclk);
	//
	//tcnt = (pclk/66);//1S
	tcnt = 90; //90US
	//clk_enable(timer_clock);
	//
	//__raw_writel(sTCNTB, timer_base+rTCNTB1);
	//__raw_writel(sTCMPB, timer_base+rTCMPB1);
	__raw_writel(tcnt, timer_base+rTCNTB1);
	__raw_writel(0, timer_base+rTCMPB1);
	//
	uTemp1 = __raw_readl(timer_base+rTINT_CSTAT);
	//uTemp1 = (uTemp1 &(~(0x41)))|(1<<0)|(1<<6);
	uTemp1 = uTemp1|(1<<1)|(1<<6);
	__raw_writel(uTemp1, timer_base+rTINT_CSTAT); 
	//

	uTemp3 = __raw_readl(timer_base+rTCON);
	uTemp3 = (uTemp3 &(~(0x00f10)))|(uAutoreload<<11)|(uEnInverter<<10)|(1<<9)|(1<<8);
	__raw_writel(uTemp3, timer_base+rTCON); // timer1 manual update

	//
	//uTemp3 = (uTemp3 &(~(0x00f10)))|(uAutoreload<<11)|(uEnInverter<<10)|(0<<9)|(1<<8);
	uTemp3 = uTemp3 & (~(0x2)<<8);
	__raw_writel(uTemp3, timer_base+rTCON);// timer1 start
	//
	printk("timer1 init success!\n");
	//
	
}
#endif
//

static int s3c6410timer_open(struct inode *inode, struct file *file)
{
	//s3c6410timer_start();
	//
	try_module_get(THIS_MODULE);
	printk( KERN_INFO DEVICE_NAME " opened!\n");
}
//

static int s3c6410timer_release(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);
	s3c6410timer_stop();
	printk(DEVICE_NAME" released!\n");
	return 0;	
}
//

static ssize_t s3c6410timer_write(struct file *file, const char __user *data,
				size_t len, loff_t *ppos)
{

}
//

static long s3c6410timer_ioctl(struct file *file,	unsigned int cmd,
							unsigned long arg)
{
	;	
}
//

/* kernel interface */
static const struct file_operations s3c6410timer_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.open		= s3c6410timer_open,
	.release	= s3c6410timer_release,
	.write		= s3c6410timer_write,
	.unlocked_ioctl	= s3c6410timer_ioctl,	
};
//

static struct miscdevice s3c6410timer_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= DEVICE_NAME,  //s3c6410_timer
	.fops		= &s3c6410timer_fops,	
};
//

static int __init s3c6410timer_init(void)
{
	int ret;
	//
	printk (DEVICE_NAME"\tinitialized start .\n");
	//timer_base = ioremap(S3C6410_TIMERREG, 72);
	timer_base = S3C_VA_TIMER;
	//printk("S3C_VA_TIMER = %x\n", S3C_VA_TIMER);
	if (timer_base == NULL) 
	{
		printk(KERN_ERR "Failed to remap register block\n");
		ret = -EINVAL;
	}
	//
/*
	//timer_clock = clk_get(NULL, "timers");
	timer_clock = clk_get(NULL, "pclk");
	if (!timer_clock) 
	{
		printk(KERN_ERR "failed to get timer clock source\n");
		return -ENOENT;
	}
	clk_enable(timer_clock);
*/
	//
	/* Setup tick interrupt */
	//ret = request_irq(TIMER_IRQ, s3c6410timer_irq, IRQF_DISABLED, "s3c6410timer", NULL);
	ret = request_irq(TIMER_IRQ, s3c6410timer_irq, IRQF_TRIGGER_HIGH, "s3c6410_timer", NULL);
	if (ret)
	{
		//iounmap(timer_base);
		printk(KERN_WARNING "ret : %d !\n", ret);
		printk(KERN_WARNING "s3c6410: Failed to get IRQ : %d !\n", TIMER_IRQ);
	}
	//
	s3c6410timer_start();
	//
	printk (DEVICE_NAME"\tinitialized finished .\n");
	//
	return ret;
}
//

static void __exit s3c6410timer_exit(void)
{
	free_irq(TIMER_IRQ, NULL);
/*	iounmap(timer_base);

	//
	if (timer_clock)
	{
		clk_disable(timer_clock);
		clk_put(timer_clock);
		timer_clock = NULL;
	}
*/
	//
	misc_deregister(&s3c6410timer_miscdev);	
}
//
module_init(s3c6410timer_init);
module_exit(s3c6410timer_exit);
//
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Realarm.");
//


