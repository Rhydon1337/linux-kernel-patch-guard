# linux-kernel-patch-guard

## TL;DR

Minimal patch guard for Linux kernel.

Depends on systemd and kallsyms which means it will work on most of Linux distros except embedded systems which still use init.

Tested on Linux kernel version: 4.19.91.

## How it works

Calculate hash of critical data structures and when a change occurs halt the computer with MALWARE DETECTED message.

The module has self protect mechanism which protects from anyone to patch its binary on the disk or patch the module memory.

The module has persistency mechanism, which utilizes systemd kernel module loading. The module ensures to be the first module to load by manipulating systemd module order method. It will also write itself to the reboot notifier list and it will ensure to be the last one to be called at shutdown or reboot in order to ensure its persistency (by manipulating the reboot notifier list).

The data structures which currently supported are:

* Interrupt descriptor table
* Global descriptor table
* System call table

## Limitations

There are a lot of limitations and techniques to bypass my little patch guard. However, it's not easy at all.

## Usage

cp patch_guard.ko /lib/modules/\`uname -r\`

insmod patch_guard.ko

DONE!!!