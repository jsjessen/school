#!/bin/bash

NAME='jjessen'
PROJECT='4proj'

EXE_FILE="/net/u/${NAME}/pvt/parallel_483/${PROJECT}/${PROJECT}.exe"
MACHINE_FILE="/net/u/${NAME}/pvt/machinefile.txt"

CFLAGS="-std=c99 -Wall -lm"
ARGS=""

NUM_NODES=2
PROCS_PER_NODE=1
TOTAL_NUM_PROCS=2

NODES=($(shuf -i 1-5 -n ${NUM_NODES}))

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
mpicc ${CFLAGS} -o ${EXE_FILE} $*

# Execute
mpiexec -machinefile ${MACHINE_FILE} -n ${TOTAL_NUM_PROCS} ${EXE_FILE} ${ARGS}


#To compile your OpenMP function you need to use the -fopenmp flag. 
#For instance to compile a C/OpenMP code on the glx nodes you will use "gcc -fopenmp -o <execname> <sourcefile(s)>".

#To run the executable, simply run the executable directly with its arguments like you would do if it were a serial code. 
#No need to do anything different. It is a good practice to have the user specify the number of threads as one of the executable's arguments. 

echo
