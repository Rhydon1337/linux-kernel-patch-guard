#pragma once

#define DEPMOD_PATH "/sbin/depmod"

#define MAYJOR_NUMBER 510

#define DEVICE_NAME "PatchGuard"

#define MD5_RESULT_SIZE 16

enum RETURN_CODE{
    SUCCESS,
    MALWARE_DETECTED,
};

struct ValidatorMd5 {
    char* boot_file_md5;
    char* syscall_table_md5;
    char* module_memory_md5;
    char* module_file_md5;
};