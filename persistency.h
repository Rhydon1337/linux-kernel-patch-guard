#pragma once
int persistency_validator(char* driver_path);

int persistency_boot_validator(char* driver_path);

int persistency_shutdown_validator(char* driver_path);

void register_for_boot(char* driver_path);

void register_for_shutdown(char* driver_path);