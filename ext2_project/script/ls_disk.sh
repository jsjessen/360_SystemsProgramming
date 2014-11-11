#!/bin/sh

sudo mount -o loop disk /mnt
ls -l /mnt
sudo umount /mnt
