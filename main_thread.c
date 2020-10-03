#include "main_thread.h"

#include <linux/string.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/delay.h>

#include "const.h"
#include "hash.h"
#include "persistency.h"
#include "self_protect.h"

#define SECONDS_IN_MILISEC 1000

bool g_should_stop_thread = false;

int main_validation_logic_thread(void* validators_md5) {
    struct ValidatorMd5* md5 = (struct ValidatorMd5*)validators_md5;
    
    // Ensure persistency
    md5->boot_file_md5 = register_for_boot();
	print_md5(md5->boot_file_md5);

    // Chech that no one patched our module, must be the last check
    md5->module_memory_md5 = self_protect();
	print_md5(md5->module_memory_md5);
    
    while (!g_should_stop_thread) {
        printk(KERN_INFO "Hi from kernel thread\n");
        if (MALWARE_DETECTED == persistency_validator(md5->boot_file_md5)) {
            kfree(md5->boot_file_md5);
            md5->boot_file_md5 = register_for_boot();
            printk(KERN_INFO "malware detected\n");
        }
        if (MALWARE_DETECTED == self_protect_validator(md5->module_memory_md5)) {
            printk(KERN_INFO "malware detected\n");
        }
    
        register_for_shutdown();

        msleep(SECONDS_IN_MILISEC * 10);
    }
    
    return SUCCESS;
}