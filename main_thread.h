#pragma once
#include "const.h"

#include <linux/module.h>

extern bool g_should_stop_thread;

int main_validation_logic_thread(void* validators_md5);