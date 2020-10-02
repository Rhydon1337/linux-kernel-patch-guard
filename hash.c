#include "hash.h"

#include <crypto/hash.h>
#include <linux/err.h>

#include "file.h"

#define MD5_RESULT_SIZE 16

char* get_md5(char* input, size_t len){
    struct crypto_shash* shash;
    struct shash_desc* desc;
    size_t size;
    char* result = kmalloc(MD5_RESULT_SIZE, GFP_KERNEL);

	shash = crypto_alloc_shash("md5", 0, 0);
    if (IS_ERR(shash)) {
        return NULL;
    }
    size = sizeof(struct shash_desc) + crypto_shash_descsize(shash);
    
    desc = kmalloc(size, GFP_KERNEL);
    if (NULL == desc) {
        crypto_free_shash(shash);
        return NULL;
    }
    
    desc->tfm = shash;

    crypto_shash_init(desc);
    crypto_shash_update(desc, input, len);
    crypto_shash_final(desc, result);

    crypto_free_shash(desc->tfm);
    kfree(desc);
    return result;
}

void print_md5(char* input){
	size_t i;
    char hexline[MD5_RESULT_SIZE * 2 + 1];
    memset(hexline, 0, (MD5_RESULT_SIZE * 2) + 1);
    for (i = 0; i < MD5_RESULT_SIZE; i++) {
        sprintf(hexline + (i * 2), "%02X", (u8)input[i]);
    }
    printk(KERN_INFO "MD5 result: %s\n", hexline);
}

char* get_file_md5(char* file_path){
    struct file* file;
    char* data;
    char* md5 = NULL;
    size_t file_size = get_file_size(file_path);
    if (0 == file_size) {
        return NULL;
    }
    data = kmalloc(file_size, GFP_KERNEL);
    if (NULL == data) {
        return NULL;
    }
    file = file_open(file_path, O_RDONLY, 0);
    file_read(file, 0, data, file_size);
    md5 = get_md5(data, file_size);
    kfree(data);
    file_close(file);
    return md5;
}