#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>



#define	GLOBALMEM_SIZE   	0x1000
#define	MEM_CLEAR					0x1
#define	GLOBALMEM_MAJOR		255

static int globalmem_major = GLOBALMEM_MAJOR;

struct globalmem_dev 
{
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];	
};

struct globalmem_dev dev;


static int globalmem_init(void)
{
	int result;

  printk("globalmem, Driver\n");
	dev_t devno = MKDEV(globalmem_major, 0);
	if (globalmem_major)
	{
		result = register_chrdev_region(devno, 1, "globalmem");
	}
	else
	{
		result = allov_chrdev_region(devno, 0, 1, "globalmem");
		globalmem_major = MKDEV(devno);
	}
	if (result < 0)
	{
		return result;
	}
	globalmem_setup_cdev();
  return 0;
}

static void globalmem_exit(void)
{
	cdev_del(&dev.cdev);
	unregister_chrdev_region(MKDEV(globalmem_major, 0), 1);
  printk("globalmem leave see you\n");
}

module_init(globalmem_init);
module_exit(globalmem_exit);

MODULE_AUTHOR("song");
MODULE_DESCRIPTION("this is a simple example!\n");
MODULE_ALIAS("A simple example");
MODULE_LICENSE("GPL");
