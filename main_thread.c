#include "main_thread.h"

#include <linux/string.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/module.h>

#include "const.h"
#include "hash.h"

int main_validation_logic_thread(void* validators_md5) {
    struct ValidatorMd5* md5 = (struct ValidatorMd5*)validators_md5;
    printk(KERN_INFO "Hi from kernel thread\n");
    print_md5(md5->boot_file_md5);
    while () {
        /* code */
    }
    
    return SUCCESS;
}