#!/bin/sh

sudo mount -o loop disk/mydisk /mnt
ls -l /mnt
sudo umount /mnt
