#!/bin/sh

USERNAME='jjessen'

echo
echo "Downloading..."

while [ $1 ]
do
    scp ${USERNAME}@ssh1.eecs.wsu.edu:~/parallel_483/$1 $1
    shift
done
