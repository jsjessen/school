#!/bin/bash

NAME='proj5'
EXE=${NAME}'.exe'

MAX_THREADS=32

TIMEOUT_VAL=600 #seconds

gcc -o $EXE -fopenmp main.c -lm 
if [ $? -ne 0 ]
then
    echo Compilation Error: $?
    exit 1
fi

clear

count=1
initThrows=1024
while :
do
    numThreads=1
    while [ "$numThreads" -le "$MAX_THREADS" ]
    do
        numThrows=$(($initThrows * $numThreads))

        echo "Threads = $numThreads   Throws = $numThrows"
        timeout $TIMEOUT_VAL ./$EXE $numThrows $numThreads >> 'outfile'${count}'.csv'

        if [ $? -gt 0 ]
        then
            if [ $numThreads -lt $MAX_THREADS ]
            then
                exit 0
            fi

            break
        fi

        let "numThreads <<= 1" #double threads
    done
    let "initThrows <<= 1" #double initial throws 
    ((count++))
done

echo DONE
