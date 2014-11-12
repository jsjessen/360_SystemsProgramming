#!/bin/sh

DISK=disk/mydisk

touch $DISK 
sudo mkfs.ext2 $DISK 1440
