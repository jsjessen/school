#!/bin/bash

NAME='proj5'
EXE=${NAME}'.exe'
OUTFILE='output1.csv'

MAX_THROWS=4294967296
MAX_THREADS=4

TIMEOUT_VAL=600 #seconds

gcc -o $EXE -fopenmp main.c -lm 
if [ $? -ne 0 ]
then
    echo Compilation Error: $?
    exit 1
fi

rm -f $OUTFILE
clear

numThreads=1
while [ "$numThreads" -le "$MAX_THREADS" ]
do
    numThrows=1024
    while [ "$numThrows" -le "$MAX_THROWS" ]
    do
        echo "Threads = $numThreads   Throws = $numThrows"
        timeout $TIMEOUT_VAL ./$EXE $numThrows $numThreads >> $OUTFILE

        if [ $? -gt 0 ]
        then
            echo Timeout
            break
        fi

        let "numThrows <<= 1" #double throws
    done
    let "numThreads <<= 1" #double threads
done

echo DONE
