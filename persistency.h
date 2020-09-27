bool persistency_validator(char* driver_path);

bool persistency_boot_validator(char* driver_path);

bool persistency_shutdown_validator(char* driver_path);

void register_for_boot(char* driver_path);

void register_for_shutdown(char* driver_path);