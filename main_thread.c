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
#include "global_cpu_tables_protect.h"
#include "syscall_table_protect.h"

#define SECONDS_IN_MILISEC 1000

bool g_should_stop_thread = false;

int main_validation_logic_thread(void* validators_md5) {
    struct ValidatorMd5* md5 = (struct ValidatorMd5*)validators_md5;
    
    // Ensure persistency
    md5->boot_file_md5 = register_for_boot();
	print_md5(md5->boot_file_md5);

    // Syscall table protection
    md5->syscall_table_md5 = syscall_table_protect();
	print_md5(md5->syscall_table_md5);

    // Interrupt descriptor table protection
    md5->idt_per_cpu_md5 = idt_protect();

    // Global descriptor table protection
    md5->gdt_per_cpu_md5 = gdt_protect();

    // Check that no one patch our module file
    md5->module_file_md5 = self_protect_module_file();
	print_md5(md5->module_file_md5);

    // Check that no one patched our module memory, must be the last check
    md5->module_memory_md5 = self_protect_in_memory();
	print_md5(md5->module_memory_md5);
    
    while (!g_should_stop_thread) {
        printk(KERN_INFO "Hi from kernel thread\n");
        if (MALWARE_DETECTED == persistency_validator(md5->boot_file_md5)) {
            kfree(md5->boot_file_md5);
            md5->boot_file_md5 = register_for_boot();
            printk(KERN_INFO "malware detected tried to manipulate patch guard boot persistency\n");
        }
        if (MALWARE_DETECTED == self_protect_validator(md5->module_memory_md5, md5->module_file_md5)) {
            printk(KERN_INFO "malware detected tried to patch patch guard memory or file\n");
        }
        if (MALWARE_DETECTED == syscall_table_protect_validator(md5->syscall_table_md5)) {
            printk(KERN_INFO "malware detected tried to patch syscall table\n");
        }
        if (MALWARE_DETECTED == idt_protect_validator(md5->idt_per_cpu_md5)) {
            printk(KERN_INFO "malware detected tried to patch idt\n");
        }
        if (MALWARE_DETECTED == gdt_protect_validator(md5->gdt_per_cpu_md5)) {
            printk(KERN_INFO "malware detected tried to patch gdt\n");
        }
        register_for_shutdown();
        msleep(SECONDS_IN_MILISEC * 10);
    }
    
    return SUCCESS;
}