#!/bin/sh

DISK="disk/make_disk"

if [ $# -gt 0 ]
then
    DISK="disk/$1"
fi

echo
echo "ls $DISK" 
echo

sudo mount -o loop $DISK /mnt
sudo ln -s /mnt/dir /mnt/dirlinuxLink
sudo ls -laiUR /mnt
sudo umount /mnt

echo
