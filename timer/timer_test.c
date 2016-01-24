#include <stdio.h>

#define	DEVICE_NAME "imx6_timer"

int main(void)
{
	
	int fd;
	int ch;

	fd = open("/dev/imx6_timer", 0);
	if (fd == -1) 
	{
		perror("Fail to open timer ");
		return -1;
	}
	while((ch = getchar()) == 'q')
		break;
	printf("exit!\n");
	close(fd);

	return 1;
}

