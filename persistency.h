#pragma once
int persistency_validator(char* driver_path);

int persistency_boot_validator(char* driver_path);

int persistency_shutdown_validator(char* driver_path);

char* register_for_boot(void);

void register_for_shutdown(void);