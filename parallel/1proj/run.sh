#!/bin/bash

# Usage: ./run.sh file1.c file2.c file3.c

# Recommend creating softlink to q (shortcut)
# ln -s run.sh q

PROJECT=1
USERNAME="yourname"

NUM_NODES=2
PROCS_PER_NODE=1
NUM_PROCS=2

COMP_ARGS="-std=c99"
EXE_ARGS=""

# You may want to edit this path or at least create the directories
EXE_FILE="/net/u/${USERNAME}/pvt/parallel_483/${PROJECT}proj/proj${PROJECT}.exe"
MACHINE_FILE="/net/u/${USERNAME}/pvt/machinefile.txt"

set -e # exit upon error
clear  # clear the terminal screen (view output without distraction)

# remove previous versions
rm -f ${EXE_FILE} ${MACHINE_FILE} 

# randomly select nodes and produce machinefile.txt
nodes=($(shuf -i 1-5 -n ${NUM_NODES}))
for node in "${nodes[@]}"; do
    echo "glx${node}.eecs.wsu.edu:${PROCS_PER_NODE}" >> ${MACHINE_FILE}
done

# show which nodes were selected
echo "-------------------"
cat ${MACHINE_FILE}
echo "-------------------"

# Compile
mpicc ${COMP_ARGS} -o ${EXE_FILE} $* 

# Execute
mpiexec -machinefile ${MACHINE_FILE} -n ${NUM_PROCS} ${EXE_FILE} ${EXE_ARGS}


#To compile your OpenMP function you need to use the -fopenmp flag. 
#For instance to compile a C/OpenMP code on the glx nodes you will use "gcc -fopenmp -o <execname> <sourcefile(s)>".

#To run the executable, simply run the executable directly with its arguments like you would do if it were a serial code. 
#No need to do anything different. It is a good practice to have the user specify the number of threads as one of the executable's arguments.