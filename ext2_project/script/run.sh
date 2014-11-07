#! /bin/bash

cd ..

make 

PROG="my_ext2fs"
DISK="fdimage"

$PROG $DISK
