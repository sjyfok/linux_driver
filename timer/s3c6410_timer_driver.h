#ifndef __S3C6410_TIMER_H
#define __S3C6410_TIMER_H



/***************************************************************
** ROM DATA
***************************************************************/
#define DEVICE_NAME "s3c6410_timer"
//
#define TIMER_MINOR 114
//
#define S3C6410_TIMERREG 0x7F006000
//#define TIMER_BASE ioremap(S3C6410_TIMERREG, 100)
//
#define TIMER_IRQ	IRQ_TIMER1

//
#define uPrescaler	66		//Timer1 prescaler value : [1~256]  -->1MHZ -->1US
#define uDzlen		0		//Timer1 divider value   [0~255]
#define uAutoreload 1
#define uEnInverter 0
#define sTCNTB		1000000		//[1~2^31
#define sTCMPB		1000000		//[1~2^31]
//


enum TIMER_REG
{
	rTCFG0   = 0x00,
	rTCFG1   = 0x04,
	rTCON    = 0x08,
	rTCNTB0  = 0x0C,
	rTCMPB0  = 0x10,
	rTCNTO0  = 0x14,
	rTCNTB1  = 0x18,
	rTCMPB1  = 0x1C,
	rTCNTO1  = 0x20,
	rTCNTB2  = 0x24,
	rTCMPB2  = 0x28,
	rTCNTO2  = 0x2C,
	rTCNTB3  = 0x30,
	rTCMPB3  = 0x34,
	rTCNTO3  = 0x38,
	rTCNTB4  = 0x3C,
	rTCNTO4  = 0x40,
	rTINT_CSTAT = 0x44,
	//
};
//





#endif /*  __S3C6410_TIMER_H */


