#! /bin/bash

cd ..

make 

PROG="my_ext2fs"
DISK="fdimage"

valgrind --leak-check=full $PROG $DISK $FILE
