#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/kthread.h>
#include <linux/version.h>
#include <linux/gfp.h>
#include <linux/slab.h>

#include "main_thread.h"
#include "hash.h"
#include "persistency.h"
#include "const.h"
#include "self_protect.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rhydon");

static int patch_guard_init(void)
{	
	struct ValidatorMd5* validators;
	printk(KERN_INFO "hello...\n");
	validators = kmalloc(sizeof(struct ValidatorMd5), GFP_KERNEL);
    kthread_run(main_validation_logic_thread, (void*)validators, "patch_guard_thread");
	return SUCCESS;
}
 
static void patch_guard_exit(void)
{
	printk(KERN_INFO "bye ...\n");
}

module_init(patch_guard_init);
module_exit(patch_guard_exit);
