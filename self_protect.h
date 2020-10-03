#pragma once

char* self_protect_in_memory(void);

char* self_protect_module_file(void);

int self_protect_validator(char* module_memory_md5, char* module_file_md5);