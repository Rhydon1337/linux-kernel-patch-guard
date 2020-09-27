#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   

#include "hash.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rhydon");

static int driver_init(void)
{	
	char* s = "hello";
	char* md5 = get_md5(s, strlen(s));
	print_md5(md5);
	printk(KERN_INFO "hello...\n");
	return 0;
}
 
static void driver_exit(void)
{
	printk(KERN_INFO "bye ...\n");
}

module_init(driver_init);
module_exit(driver_exit);
