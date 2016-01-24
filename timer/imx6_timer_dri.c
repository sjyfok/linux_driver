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
//#include <mach/hardware.h>
//#include <asm/mach/time.h>

//#include <mach/map.h>

//#include <plat/regs-timer.h>
//
//#include "imx6__timer_driver.h"
#define EPITCR    0x00
#define EPITSR    0x04
#define EPITLR    0x08
#define EPITCMPR  0x0c
#define EPITCNR   0x10

#define EPITCR_EN     (1 << 0)
#define EPITCR_ENMOD      (1 << 1)
#define EPITCR_OCIEN      (1 << 2)
#define EPITCR_RLD      (1 << 3)
#define EPITCR_PRESC(x)     (((x) & 0xfff) << 4)
#define EPITCR_SWR      (1 << 16)
#define EPITCR_IOVW     (1 << 17)
#define EPITCR_DBGEN      (1 << 18)
#define EPITCR_WAITEN     (1 << 19)
#define EPITCR_RES      (1 << 20)
#define EPITCR_STOPEN     (1 << 21)
#define EPITCR_OM_DISCON    (0 << 22)
#define EPITCR_OM_TOGGLE    (1 << 22)
#define EPITCR_OM_CLEAR     (2 << 22)
#define EPITCR_OM_SET     (3 << 22)
#define EPITCR_CLKSRC_OFF   (0 << 24)
#define EPITCR_CLKSRC_PERIPHERAL  (1 << 24)
#define EPITCR_CLKSRC_REF_HIGH    (1 << 24)
#define EPITCR_CLKSRC_REF_LOW   (3 << 24)
 
#define EPITSR_OCIF     (1 << 0)

#define		DEVICE_NAME "imx6_timer"
//
static void __iomem	*timer_base;
static struct clk	*timer_clock;
//

#define		TIMER_IRQ		88 //MXC_INT_EPIT1



unsigned int interrup_num;

/* interrupt handler code */
static irqreturn_t imx6_timer_irq(int irqno, void *param)
{
//	unsigned long uTemp1;

	//printk("timer int\n");
	////////////////////////////////////////////
	//
	/*
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
	/////////////////////////////////////////////////
	*/
	//
	return IRQ_HANDLED;
}
//

static void imx6_timer_stop(void)
{
	unsigned int uTemp0;
	//-----------------------Set SFRs--------------------------
	//////////////////////////////////////////////
	uTemp0 = __raw_readl(timer_base+EPITCNR);
	printk("EPITCNR: %x\n", uTemp0);
	uTemp0 = __raw_readl(timer_base+EPITCR);
	uTemp0 &= ~EPITCR_EN; 
	__raw_writel(uTemp0, timer_base+EPITCR);
	free_irq(TIMER_IRQ, NULL);


/*	uTemp0 = readl(S3C_TCON);
	uTemp0 = uTemp0 & (0xfffffeff);
	writel(uTemp0, S3C_TCON);// Timer1 stop */
	/////////////////////////////////////////////////
}
//

static int imx6_timer_start(void)
{
	int ret;
	unsigned long uTemp0;
	unsigned long uTemp1;
	unsigned long uTemp2;
	unsigned long uTemp3;

	unsigned long pclk;
	unsigned long tcnt;

	timer_clock = clk_get(NULL, "ipg_perclk");
	if (!timer_clock) 
	{
		printk(KERN_ERR "failed to get timer clock source\n");
		return -ENOENT;
	}
	clk_enable(timer_clock);
	pclk = clk_get_rate(timer_clock);
	printk("pclk %ld \n", pclk);
	
	//fixup interrupt
	if (ret != 0)
	{
		printk("Fial to request_irq\n");
		return ret;
	}
	///////////////////////////////////////////////////////////
	//
	tcnt = 90; //90US

	//timer初始状态
	__raw_writel(0x0, timer_base + EPITCR);
	__raw_writel(0xffffffff, timer_base + EPITLR);

	__raw_writel(EPITCR_EN | EPITCR_CLKSRC_REF_HIGH | EPITCR_WAITEN,	timer_base+EPITCR);

	ret = request_irq(TIMER_IRQ, imx6_timer_irq, IRQF_TRIGGER_HIGH, "imx6__timer", NULL);


	//
	//-----------------------Set SFRs--------------------------
	uTemp0 = __raw_readl(timer_base+EPITCR);
	printk("EPITCR : %lx\n", uTemp0);
	uTemp1 = __raw_readl(timer_base+EPITSR);
	printk("EPITSR: %lx\n", uTemp1);
	uTemp2 = __raw_readl(timer_base+EPITLR);
	printk("EPITLR: %lx\n", uTemp2);
	uTemp3 = __raw_readl(timer_base+EPITCMPR);
	printk("EPITCMPR: %lx\n", uTemp3);
	uTemp0 = __raw_readl(timer_base+EPITCNR);
	printk("EPITCNR: %x\n", uTemp0);
/*	uTemp0 = __raw_readl(S3C_TCON);
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
	//////////////////////////////////////////////////////////////////
	*/
/*
	uTemp3 = __raw_readl(S3C_TCON);
	uTemp3 &= ~0xf1<<8;
	uTemp3 |= 0xb<<8;//disable deadzone,auto-reload,inv-off,update TCNTB0&TCMPB0,start timer 1
	__raw_writel(uTemp3, S3C_TCON);// timer1 start
	//
	uTemp3 &= ~0x2<<8;//clear manual update bit
	__raw_writel(uTemp3, S3C_TCON);
*/
///////////////////////////////////////////////////////////////////
	//
	/*
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
	///////////////////////////////////////////////////////////////////
	*/
	
}
/*
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
*/
//

static int imx6_timer_open(struct inode *inode, struct file *file)
{
	printk(DEVICE_NAME " is opened!\n");
//	imx6_timer_start();
	return 0;
}
//

static int imx6_timer_close(struct inode *inode, struct file *file)
{
	//imx6_timer_stop();
	printk(DEVICE_NAME" released!\n");
	return 0;	
}
//

static ssize_t imx6_timer_write(struct file *file, const char __user *data,
				size_t len, loff_t *ppos)
{
	return 0;
}
//

static long imx6_timer_ioctl(struct file *file,	unsigned int cmd,
							unsigned long arg)
{
	return 0;	
}
//

/* kernel interface */
static const struct file_operations imx6_timer_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.open		= imx6_timer_open,
	.release	= imx6_timer_close,
	.unlocked_ioctl	= imx6_timer_ioctl,	
};
//

static struct miscdevice imx6_timer_miscdev = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= DEVICE_NAME,  //imx6__timer
	.fops		= &imx6_timer_fops,	
};
//

static int __init imx6_timer_init(void)
{
	int ret;
	printk (DEVICE_NAME"\tinitialized start .\n");
//	timer_base = ioremap(EPIT1_BASE_ADDR, 72);
//	printk("timer_base = %p\n", timer_base);
//	if (timer_base == NULL) 
//	{
//		printk(KERN_ERR "Failed to remap register block\n");
//		ret = -EINVAL;
//	}
	ret = misc_register(&imx6_timer_miscdev);
	if (ret == -1)
	{
		printk("Fail to register!\n");		
	}

	printk (DEVICE_NAME"\tinitialized finished .\n");
	
	return ret;
}

static void __exit imx6_timer_exit(void)
{
//	free_irq(TIMER_IRQ, NULL);
//	iounmap(timer_base);
/*	if (timer_clock)
	{
		clk_disable(timer_clock);
		clk_put(timer_clock);
		timer_clock = NULL;
	}*/
	misc_deregister(&imx6_timer_miscdev);
	printk(DEVICE_NAME "exit\n");
}
//
module_init(imx6_timer_init);
module_exit(imx6_timer_exit);
//
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Realarm.");
//


