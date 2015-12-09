#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
//
//#define DEV_NAME 	"/dev/sjzc_gpo"
#define DEV_NAME 	"/dev/eoas_leds"
//#define GPO_NUM		11
#define GPO_NUM		2
//
int main(int argc, char * argv)
{
	int i, n, fd;

	fd = open(DEV_NAME, O_RDWR);
	if (fd < 0)
	{
		printf("can't open %s!\n",DEV_NAME);
		exit(1);
	}
	//
	while(1) 
	{
		ioctl(fd, 1, 1);
		sleep(1);
		ioctl(fd, 0, 1);
		sleep(1);
		/*for(i=0; i<GPO_NUM; i++)
		{		
			ioctl(fd, 1, i);
		}
		sleep(1);
		for(i=0; i<GPO_NUM; i++)
		{		
			ioctl(fd, 0, i);
		}
		sleep(1);
		printf("on_off one time\n");*/
	}
	//
	close(fd);
	//
	return 0;
}
