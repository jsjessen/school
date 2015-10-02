#!/bin/sh

echo
echo "Downloading..."

while [ $1 ]
do
    scp jjessen@ssh1.eecs.wsu.edu:~/parallel_483/$1 $1
    shift
done
