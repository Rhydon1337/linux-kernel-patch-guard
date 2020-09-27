#include "hash.h"

#define MD5_RESULT_SIZE 16

char* get_md5(char* input, size_t len){
    struct scatterlist sg;
    struct hash_desc desc;
    char* result = kmalloc(MD5_RESULT_SIZE, GFP_KERNEL);
    memset(result, 0, MD5_RESULT_SIZE);
    sg_init_one(&sg, input, len);
    desc.tfm = crypto_alloc_hash("md5", 0, CRYPTO_ALG_ASYNC);

    crypto_hash_init(&desc);
    crypto_hash_update(&desc, &sg, len);
    crypto_hash_final(&desc, result);

    crypto_free_hash(desc.tfm);
    
    return result;
}