#!/bin/sh

sudo mount -o loop disk/mydisk /mnt
sudo ls -laiUR /mnt
sudo umount /mnt
