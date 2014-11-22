#!/bin/sh

DISK="disk/mydisk"

if [ $# -gt 0 ]
then
    DISK="disk/$1"
fi

echo
echo "ls $DISK" 
echo

sudo mount -o loop $DISK /mnt
sudo ls -laiUR /mnt
sudo umount /mnt

echo
