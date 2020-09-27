#include "persistency.h"

#include <linux/string.h>
#include <linux/gfp.h>
#include <linux/slab.h>

#include "file.h"

#define MODULES_BOOT_CONF "/etc/modules-load.d/"
#define BOOT_FILE_EXTENSION ".conf"
#define MAX_FILENAME_LENGTH 255

char* get_file_name(void){
    int i = 0;
    size_t boot_file_extension_len;
    char* file_name = kmalloc(MAX_FILENAME_LENGTH + 1, GFP_KERNEL);
    boot_file_extension_len = strlen(BOOT_FILE_EXTENSION);
    memcpy(file_name + MAX_FILENAME_LENGTH - boot_file_extension_len, BOOT_FILE_EXTENSION, boot_file_extension_len);
    for (; i < MAX_FILENAME_LENGTH - boot_file_extension_len; ++i){
        file_name[i] = 'a';
    }
    return file_name;
}

void register_for_boot(char* driver_path) {
    char* file_name = get_file_name();
    //file_open(file_name, )
    kfree(file_name);
}