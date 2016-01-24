/*
****************************************Copyright (c)**************************************************
**                               
**
**------------------------------------- File Info ------------------------------------------------------
** File name:           main.c
** Last modified Date:  2012-02-24
** Last Version:        1.0
** Descriptions:        Test program for KEY TEST.
**------------------------------------------------------------------------------------------------------
** Created by:          GongQingmin
** Created date:        2012-02-24
** Version:             1.0
** Descriptions:        Preliminary version.
**
**------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
********************************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
//#include <sys/time.h>
//#include <sys/stat.h>


//#define DEVICE_NAME "/dev/sjzc_gpi"
#define DEVICE_NAME "/dev/eoas_gpio"



/*
*******************************************************************************************************
** Function name: main()
** Descriptions	: Read KEY's value and Pressed times.
** Input	: NONE
** Output	: NONE
** Created by	: GongQingmin
** Created Date	: 2012-02-24
**-----------------------------------------------------------------------------------------------------
** Modified by	:
** Modified Date: 
**-----------------------------------------------------------------------------------------------------
*******************************************************************************************************
*/
int main(int argc, char **argv)
{
	int fd;
	int result;
	int key_value;
	int times = 1;
	int i;
	char read_buf[2];
	//
	fd = open(DEVICE_NAME, 0);
	if (fd < 0)
	{
		perror("Failed to open\n");
		exit(1);
	}
	
	while(1)
	{	
		result = read(fd, read_buf, 2);
		if(result > 0)
		{
			//printf("There is some key pressed.\n");
			printf("Power down.\n");
		}
		else
		{
			printf("Power up\n");
		}
		//sleep(1);
	}

exit:
	printf("close\n");
	close(fd);
	return 0;
}

