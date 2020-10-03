#include "persistency.h"

#include <linux/string.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/kallsyms.h>
#include <linux/umh.h>

#include "file.h"
#include "const.h"
#include "hash.h"

#define MODULES_BOOT_CONF_DIR "/etc/modules-load.d/"
#define BOOT_FILE_EXTENSION ".conf"
#define MAX_FILENAME_LENGTH 255

typedef void (*ksys_sync)(void);

char* BOOT_FILE_MD5 = NULL;

int shutdown_notifier(struct notifier_block *nb, unsigned long action, void *data) {
    char* md5 = register_for_boot();
    if (md5 != NULL) {
        kfree(md5);
    }
    return NOTIFY_DONE;
}

char* get_file_path(void){
    int i = 0;
    size_t boot_file_extension_len;
    char* file_full_path;
    char* file_name = kmalloc(MAX_FILENAME_LENGTH + 1, GFP_KERNEL);
    memset(file_name, 0, MAX_FILENAME_LENGTH + 1);
    boot_file_extension_len = strlen(BOOT_FILE_EXTENSION);
    memcpy(file_name + MAX_FILENAME_LENGTH - boot_file_extension_len, BOOT_FILE_EXTENSION, boot_file_extension_len);
    for (; i < MAX_FILENAME_LENGTH - boot_file_extension_len; ++i){
        file_name[i] = 'a';
    }
    file_full_path = kmalloc(strlen(MODULES_BOOT_CONF_DIR) + strlen(file_name) + 1, GFP_KERNEL);
    memset(file_full_path, 0, strlen(MODULES_BOOT_CONF_DIR) + strlen(file_name) + 1);
    memcpy(file_full_path, MODULES_BOOT_CONF_DIR, strlen(MODULES_BOOT_CONF_DIR));
    memcpy(file_full_path + strlen(MODULES_BOOT_CONF_DIR), file_name, strlen(file_name));
    kfree(file_name);
    return file_full_path;
}

int persistency_validator(char* boot_file_md5) {
    char* file_full_path = get_file_path();
    char* file_md5 = get_file_md5(file_full_path);
    int status = SUCCESS;
    if (NULL == file_md5) {
        status = MALWARE_DETECTED;
        goto free_file_path;
    }

    if (SUCCESS != memcmp(file_md5, boot_file_md5, MD5_RESULT_SIZE)) {
        status = MALWARE_DETECTED;
        goto free_md5;
    }

free_md5:
    kfree(file_md5);
free_file_path:
    kfree(file_full_path);
    register_for_shutdown();
    return status;
}

char* register_for_boot() {
    struct file* file;
    char* md5 = NULL;
    char *hargv[] = {DEPMOD_PATH, "-a", NULL};
    char *henvp[] = {"HOME=/", "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL};
    char* file_full_path = get_file_path();
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
    file_sync(file);
    file_close(file);
    ((ksys_sync)kallsyms_lookup_name("ksys_sync"))(); 
    if (0 != call_usermodehelper(DEPMOD_PATH, hargv, henvp, UMH_WAIT_PROC)) {
        printk("Failed calling depmod\n");
    }
    BOOT_FILE_MD5 = md5;
release_resources:
    kfree(file_full_path);
    return md5;
}

void register_for_shutdown(){
    struct blocking_notifier_head* reboot_notifier_list = (struct blocking_notifier_head*)kallsyms_lookup_name("reboot_notifier_list"); 
    struct notifier_block* notifier_ptr;
    struct notifier_block* notifier_prev_ptr;
    struct notifier_block* notifier = kmalloc(sizeof(struct notifier_block), GFP_KERNEL);
    notifier->next = NULL;
    notifier->notifier_call = shutdown_notifier;
    notifier->priority = 0;
    notifier_ptr = reboot_notifier_list->head;
    notifier_prev_ptr = reboot_notifier_list->head;
    if (NULL == notifier_ptr) {
        reboot_notifier_list->head = notifier;
    }
    else {
        if ((unsigned long)notifier_ptr->notifier_call == (unsigned long)shutdown_notifier && NULL != notifier_ptr->next) {
            reboot_notifier_list->head = notifier_ptr->next;
            kfree(notifier_ptr);
            notifier_ptr = reboot_notifier_list->head;
            notifier_prev_ptr = reboot_notifier_list->head;
        }
        while (NULL != notifier_ptr->next){
            if ((unsigned long)notifier_ptr->notifier_call == (unsigned long)shutdown_notifier && NULL == notifier_ptr->next) {
                kfree(notifier);
                return;
            }        
            if ((unsigned long)notifier_ptr->notifier_call == (unsigned long)shutdown_notifier && NULL != notifier_ptr->next) {
                notifier_prev_ptr->next = notifier_ptr->next;
                kfree(notifier_ptr);
                notifier_ptr = notifier_prev_ptr;

            }
            notifier_prev_ptr = notifier_ptr;
            notifier_ptr = notifier_ptr->next;
        }
        if ((unsigned long)notifier_ptr->notifier_call == (unsigned long)shutdown_notifier && NULL == notifier_ptr->next) {
            kfree(notifier);
            return;
        }
        notifier_ptr->next = notifier;    
    }
}