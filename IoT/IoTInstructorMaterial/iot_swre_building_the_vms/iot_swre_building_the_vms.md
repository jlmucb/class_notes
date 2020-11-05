Author: Troy Shurtleff
Copyright 2019 BAE Systems
SPDX-License-Identifier: Apache-2.0

---
Approved for public release; unlimited distribution
Not export controlled per ES-FL-091619-0203

---
# Building the Variable Message Sign System

This document and the associated source code is intended for course instructors only.  There are no spoilers here, but the steps here are not a part of the reverse engineering exercise.

The steps outlined here were already performed on the delivered Reverse Engineering VM.  They may be followed if modifications to the VMS are required.

This document is accompanied by the `vms_config.tgz` file which contains the Buildroot configuration for the VMS.  The contents of the `vms_config.tgz` are identified below:

```
.
├── .config ← The Buildroot configuration file for the VMS
└── overlay ← The contents are overlaid on the root of the VMS filesystem
    ├── etc
    │   ├── init.d
    │   │   └── S60signApp ← Startup script for the signApp
    │   └── network
    │       └── interfaces ← Buildroot did not properly configure this file
    └── usr
        └── sbin
            └── signApp ← The ELF file for the VMS administrative interface
```

The permissions on the contents of the `vms_config.tgz` file are set correctly.  Care should be taken when extracting this file to preserve the permissions (i.e. use the `-p` flag).

## Load the Variable Message Sign Buildroot Configuration

First, copy the `vms_config.tgz` file to the ~student/buildroot-2019.02.4 directory on the Reverse Engineering VM.  Using the VirtualBox File Manager is probably the easiest way to do the transfer.

Next, untar/zip the `vms_config.tgz` file with the following commands.  Make sure you untar/zip the file in the ~student/buildroot-2019.02.4 directory.

```
student@revm:~$ cd buildroot-2019.02.4/
student@revm:~/buildroot-2019.02.4$ tar -xzpf vms_config.tgz 
```

## Run the Variable Message Sign Buildroot Build

To start the Buildroot build run the following command.  This could take 35 to 45 minutes to complete.  Subsequent builds are performed incrementally and complete much faster.

```
student@revm:~/buildroot-2019.02.4$ date;make > make.log 2>&1;date
```

Note that the primary command above is `make`.  To time the build the `date` command is run before and after.  Also, the build output is redirected to make.log.

## Additional Configuration Steps to Make the Environment Match the Exercise

To preserve the original output images, make a copy of them in the `~student/vms_target` folder and set the permissions as shown below.  The exercise starts the VMS with the images in the `~student/vms_target` folder.

```
student@revm:~/buildroot-2019.02.4$ cd ~
student@revm:~$ mkdir vms_target
student@revm:~$ cp ~/buildroot-2019.02.4/output/images/rootfs.ext2 vms_target/
student@revm:~$ cp ~/buildroot-2019.02.4/output/images/versatile-pb.dtb vms_target/
student@revm:~$ cp ~/buildroot-2019.02.4/output/images/zImage vms_target/
student@revm:~$ chmod 444 vms_target/versatile-pb.dtb 
student@revm:~$ chmod 444 vms_target/zImage 
student@revm:~$ 
```

Also, create a `vms_ghidra` folder to contain the student's Ghidra project as shown below.
```
student@revm:~$ mkdir vms_ghidra
```
Instructions for building the VMS are included in the "Building the Vulnerable signApp" document.

## How to Modify the Variable Message Sign Buildroot Configuration

To modify the VMS Buildroot configuration launch the GUI configuration tool using the following command.  Configuration modifications made using this tool affect the `.config` configuration file.

```
student@revm:~/buildroot-2019.02.4$ make gconfig
```

---
Author: Troy Shurtleff
Copyright 2019 BAE Systems
SPDX-License-Identifier: Apache-2.0

Approved for public release; unlimited distribution
Not export controlled per ES-FL-091619-0203