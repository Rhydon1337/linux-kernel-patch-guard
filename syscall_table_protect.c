#include "syscall_table_protect.h"

#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/unistd.h>

#include "hash.h"
#include "const.h"

char* syscall_table_protect(void) {
    void* syscall_table = (void *)kallsyms_lookup_name("sys_call_table");
    // The number of enries are: {Last_syscall_number} * sizeof(unsigned long)
    return get_md5(syscall_table, __NR_rseq * sizeof(unsigned long));
}

int syscall_table_protect_validator(char* syscall_table_md5) {
    char* current_syscall_table_md5 = syscall_table_protect();
    int status = SUCCESS;
    if (SUCCESS != memcmp(current_syscall_table_md5, syscall_table_md5, MD5_RESULT_SIZE)) {
        status = MALWARE_DETECTED;
    }
    kfree(current_syscall_table_md5);
    return status;
}