#! /bin/bash

qemu-system-x86_64 -hda disk.img -m 2G -nic user,hostfwd=tcp::5555-:22 -nographic
