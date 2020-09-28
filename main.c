#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   

#include "hash.h"
#include "persistency.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rhydon");

static int driver_init(void)
{	
	char* s = "hello";
	char* md5 = get_md5(s, strlen(s));
	print_md5(md5);
	char* boot_md5 = register_for_boot();
	print_md5(boot_md5);
	char* boot_md512 = get_file_md5("/etc/modules");
	print_md5(boot_md512);
	register_for_shutdown();
	printk(KERN_INFO "hello...\n");
	return 0;
}
 
static void driver_exit(void)
{
	printk(KERN_INFO "bye ...\n");
}

module_init(driver_init);
module_exit(driver_exit);
