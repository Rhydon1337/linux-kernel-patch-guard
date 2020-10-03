#include "self_protect.h"

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/utsname.h>

#include "hash.h"
#include "const.h"

void* g_module_base_address;

unsigned int g_module_base_size;

#define MODULE_DIR "/lib/modules/"

char* get_module_path(void) {
    size_t offset = 0;
    size_t size = strlen(MODULE_DIR) + strlen(utsname()->release) + strlen("/") +  strlen(THIS_MODULE->name) + strlen(".ko")+ 1;
    char* file_path = kmalloc(size , GFP_KERNEL);
    memset(file_path, 0, size);
    size = strlen(MODULE_DIR);
    memcpy(file_path + offset, MODULE_DIR, size);
    offset += size;

    size = strlen(utsname()->release);
    memcpy(file_path + offset, utsname()->release, size);
    offset += size;
    
    size = strlen("/");
    memcpy(file_path + offset, "/", size);
    offset += size;

    size = strlen(THIS_MODULE->name);
    memcpy(file_path + offset, THIS_MODULE->name, size);
    offset += size;

    size = strlen(".ko");
    memcpy(file_path + offset, ".ko", size);
    offset += size;
    
    return file_path;
}

int self_protect_validator(char* module_memory_md5, char* module_file_md5) {
    char* file_path;
    char* file_md5;
    int status = SUCCESS;
    char* current_module_md5 = get_md5(g_module_base_address, g_module_base_size);
    if (SUCCESS != memcmp(current_module_md5, module_memory_md5, MD5_RESULT_SIZE)) {
        status = MALWARE_DETECTED;
    }
    kfree(current_module_md5);
    file_path = get_module_path();
    file_md5 = get_file_md5(file_path);
    if (NULL == file_md5 || SUCCESS != memcmp(file_md5, module_file_md5, MD5_RESULT_SIZE)) {
        status = MALWARE_DETECTED;
    }
    kfree(file_path);
    kfree(file_md5);
    return status;
}

void hide_me(void) {
    list_del(&THIS_MODULE->list);             //remove from procfs
    kobject_del(&THIS_MODULE->mkobj.kobj);    //remove from sysfs
    THIS_MODULE->sect_attrs = NULL;
    THIS_MODULE->notes_attrs = NULL;
}

char* self_protect_module_file(void) {
    char* file_path = get_module_path();
    char* file_md5 = get_file_md5(file_path);
    kfree(file_path);
    return file_md5;
}

char* self_protect_in_memory(void) {    
    g_module_base_address = THIS_MODULE->core_layout.base;
    g_module_base_size = THIS_MODULE->core_layout.size;
    hide_me();
    return get_md5(g_module_base_address, g_module_base_size);
}