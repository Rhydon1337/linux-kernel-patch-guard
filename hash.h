#pragma once

#include <linux/types.h>

void print_md5(char* input);

char* get_md5(char* input, size_t len);

char* get_file_md5(char* file_path);