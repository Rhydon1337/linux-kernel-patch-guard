#pragma once

char** idt_protect(void);

int idt_protect_validator(char** idt_per_cpu_md5);

char** gdt_protect(void);

int gdt_protect_validator(char** gdt_per_cpu_md5);