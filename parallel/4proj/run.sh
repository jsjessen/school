#!/bin/bash

PROJECT='4proj'

EXE_FILE="/net/u/jjessen/pvt/parallel_483/${PROJECT}/${PROJECT}.exe"
MACHINE_FILE="/net/u/jjessen/pvt/machinefile.txt"

CFLAGS="-std=c99 -lm"
#DEBUG_FLAGS="-D DEBUG2"
ARGS=""

NUM_NODES=4
PROCS_PER_NODE=4
TOTAL_NUM_PROCS=16

NODES=($(shuf -i 2-5 -n ${NUM_NODES}))

set -e
clear

rm -f ${EXE_FILE} ${MACHINE_FILE}

for node in "${NODES[@]}"; do
    echo "glx${node}.eecs.wsu.edu:${PROCS_PER_NODE}" >> ${MACHINE_FILE}
done

echo "-------------------"
cat ${MACHINE_FILE}
echo "-------------------"

# Compile
mpicc ${CFLAGS} ${DEBUG_FLAGS} -o ${EXE_FILE} $* 

n=128
while true
do
    # Execute
    mpiexec -machinefile ${MACHINE_FILE} -n ${TOTAL_NUM_PROCS} ${EXE_FILE} ${ARGS} $n
    let "n <<= 1"
done

#To compile your OpenMP function you need to use the -fopenmp flag. 
#For instance to compile a C/OpenMP code on the glx nodes you will use "gcc -fopenmp -o <execname> <sourcefile(s)>".

#To run the executable, simply run the executable directly with its arguments like you would do if it were a serial code. 
#No need to do anything different. It is a good practice to have the user specify the number of threads as one of the executable's arguments. 

#echo
