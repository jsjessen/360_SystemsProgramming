#!/bin/sh

echo
echo "Uploading..."

while [ $2 ]
do
    scp $2 james_jessen@cs360.eecs.wsu.edu:~/$1/$2
    shift
done
