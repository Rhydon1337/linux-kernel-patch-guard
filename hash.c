#include "hash.h"

#include <crypto/hash.h>
#include <linux/err.h>

#define MD5_RESULT_SIZE 16

char* get_md5(char* input, size_t len){
    struct crypto_shash* shash;
    struct shash_desc* desc;
    size_t size;
    char* result = kmalloc(MD5_RESULT_SIZE, GFP_KERNEL);

	shash = crypto_alloc_shash("md5", 0, 0);
    if (IS_ERR(shash))
    {
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
    char hexbyte[MD5_RESULT_SIZE] = "";
    char hexline[MD5_RESULT_SIZE * 2 + 1] = "";
    memset(hexline, 0, MD5_RESULT_SIZE * 2 + 1);
    for (i = 0; i< MD5_RESULT_SIZE; i++) { // traverse through mem until count is reached
        sprintf(hexbyte + (i * 2), "%02X", (u8)input[i]); // add current byte to hexbyte
    }
    printk(KERN_INFO "MD5 result: %s\n", hexbyte);
}