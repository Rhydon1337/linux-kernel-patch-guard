#include "global_cpu_tables_protect.h"

#include <linux/interrupt.h>
#include <linux/cpumask.h>
#include <asm/desc.h>
#include <linux/slab.h>

#include "hash.h"
#include "const.h"

void get_idt_md5(void *info){
    struct desc_ptr* idt = NULL;
    char* idt_md5;
    store_idt(&__IDT_register);
    idt_md5 = get_md5((char*)idt->address, idt->size);
    memcpy(info, idt_md5, MD5_RESULT_SIZE);
    kfree(idt_md5);
}

char** idt_protect(void) {
    size_t i = 0;
    size_t number_of_cpus = num_online_cpus();
    
    char** idt_md5_per_cpu = kmalloc(sizeof(char*) * number_of_cpus, GFP_KERNEL);
     for (; i < number_of_cpus; i++) {
        idt_md5_per_cpu[i] = kmalloc(MD5_RESULT_SIZE, GFP_KERNEL);
        smp_call_function_single(i, get_idt_md5,  idt_md5_per_cpu[i], true);
    }
    return idt_md5_per_cpu;
}

int idt_protect_validator(char** idt_per_cpu_md5) {
    size_t number_of_cpus = num_online_cpus();
    size_t i = 0;
    char** current_idt_per_cpu_md5 = idt_protect();
    int status = SUCCESS;
    for (; i < number_of_cpus; i++) {
        if (SUCCESS != memcmp(current_idt_per_cpu_md5[i], idt_per_cpu_md5[i], MD5_RESULT_SIZE)) {
            status = MALWARE_DETECTED;
        }
    }
    i = 0;
    for (; i < number_of_cpus; i++) {
        kfree(current_idt_per_cpu_md5[i]);
    }
    kfree(current_idt_per_cpu_md5);
    return status;
}


char** gdt_protect(void) {
    size_t i = 0;
    size_t number_of_cpus = num_online_cpus();
    struct desc_struct* gdt;
    char** gdt_per_cpu = kmalloc(sizeof(char*) * number_of_cpus, GFP_KERNEL);
    for (; i < number_of_cpus; i++) {
        gdt = get_cpu_gdt_ro(i);
        gdt_per_cpu[i] = get_md5((char*)gdt, GDT_ENTRIES * sizeof(struct desc_struct));
    }
    return gdt_per_cpu;
}

int gdt_protect_validator(char** gdt_per_cpu_md5) {
    size_t number_of_cpus = num_online_cpus();
    size_t i = 0;
    char** current_gdt_per_cpu_md5 = gdt_protect();
    int status = SUCCESS;
    for (; i < number_of_cpus; i++) {
        if (SUCCESS != memcmp(current_gdt_per_cpu_md5[i], current_gdt_per_cpu_md5[i], MD5_RESULT_SIZE)) {
            status = MALWARE_DETECTED;
        }
    }
    i = 0;
    for (; i < number_of_cpus; i++) {
        kfree(current_gdt_per_cpu_md5[i]);
    }
    kfree(current_gdt_per_cpu_md5);
    return status;
}