#! /bin/bash

make 

PROG="my_ext2fs"
DISK="disk/mydisk"

touch $DISK
sudo mkfs.ext2 $DISK 1440

valgrind --leak-check=full $PROG $DISK $FILE
