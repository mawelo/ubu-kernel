# ubu-kernel
Maintaine your own Ubuntu Linux Kernel on your system

This tool will help you to maintane your installed Ubuntu Mainline Linux Kernels.
At this time you can list and remove installed kernel versionen. But also list online available ones
and get the change list. A download function to get an online available kernel will follow soon.

### Here some examples of the usage:

List installed kernel versions + Debian package names
```
./lnxkern --action=list
5.11.4-051104 : 
        |- linux-headers-5.11.4-051104
        |- linux-headers-5.11.4-051104-generic
        |- linux-image-unsigned-5.11.4-051104-generic
        |- linux-modules-5.11.4-051104-generic
5.11.6-051106 : 
        |- linux-headers-5.11.6-051106
        |- linux-headers-5.11.6-051106-generic
        |- linux-image-unsigned-5.11.6-051106-generic
        |- linux-modules-5.11.6-051106-generic
5.4.0-66 : 
        |- linux-headers-5.4.0-66-generic
        |- linux-hwe-5.4-headers-5.4.0-66
        |- linux-image-5.4.0-66-generic
        |- linux-modules-5.4.0-66-generic
        |- linux-modules-extra-5.4.0-66-generic
5.4.0-67 : 
        |- linux-headers-5.4.0-67-generic
        |- linux-hwe-5.4-headers-5.4.0-67
        |- linux-image-5.4.0-67-generic
        |- linux-modules-5.4.0-67-generic
        |- linux-modules-extra-5.4.0-67-generic
```
Usage a filter if you like to see only a portion:
```
./lnxkern --action=list --filter=5.1
```

If you like to remove kernels:
```
./lnxkern --action=remove --filter=5.11.4
sudo apt remove -y  linux-headers-5.11.4-051104 linux-headers-5.11.4-051104-generic linux-image-unsigned-5.11.4-051104-generic linux-modules-5.11.4-051104-generic
sudo rm -rf /lib/modules/5.11.4-051104*
```
You can now execute the given commands on stdout to remove the packages and the modules.

List some of the availabe kernels from  https://kernel.ubuntu.com/~kernel-ppa/mainline :
```
./lnxkern --action=list-online --filter='v5\.[1-9][1-9].*'
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11 : v5.11
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11-rc1 : v5.11-rc1
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11-rc2 : v5.11-rc2
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11-rc3 : v5.11-rc3
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11-rc4 : v5.11-rc4
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11-rc5 : v5.11-rc5
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11-rc6 : v5.11-rc6
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11-rc7 : v5.11-rc7
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11.1 : v5.11.1
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11.2 : v5.11.2
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11.3 : v5.11.3
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11.4 : v5.11.4
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11.5 : v5.11.5
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.11.6 : v5.11.6
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.12-rc1 : v5.12-rc1
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.12-rc1-dontuse : v5.12-rc1-dontuse
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.12-rc2 : v5.12-rc2
 |- v5 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.12-rc3 : v5.12-rc3
```

Get all CHANGE descriptions for a group of kernel versions:
```
./lnxkern --action=changes --filter='v5\.[1-2]\..*'|more
v5 : v5.1.10 : https://kernel.ubuntu.com/~kernel-ppa/mainline/v5.1.10/CHANGES : 
Adam Ludkiewicz (1):
      i40e: Queues are reserved despite "Invalid argument" error

Amir Goldstein (2):
      ovl: do not generate duplicate fsnotify events for "fake" path
      ovl: support stacked SEEK_HOLE/SEEK_DATA
...
```

...more functions will bee added :-)

# depends on
A big thank you to Hana Dusíková for:
ctre-unicode.hpp        --> go to https://github.com/hanickadot/compile-time-regular-expressions
and place this file into the ./includes directory

A big thank you to Niels Lohmann for:
json.hpp --> go to https://github.com/nlohmann/json
and place this file into the ./includes directory

# build steps
just execute ```make``` after you placed all files needed as a dependency.

