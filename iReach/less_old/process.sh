#!/bin/bash

PERIOD=500
OVERLAP=100

# This script will extract statistical features from the raw data
# and output it into well named files of the form: sub#-mvt#-node#

# If this var is true, parsed raw data will be similarly outputted as well 
PARSE_RAW=1

FIRST_SUBJECT=3  # First subject with valid data

#IN_DIR=./data/raw
#OUT_DIR=./data
IN_DIR=/media/jsj/DROPSHIP/data/raw
EXTRACT_OUT_DIR=/media/jsj/DROPSHIP/data/features-period${PERIOD}-overlap${OVERLAP}
PARSE_OUT_DIR=/media/jsj/DROPSHIP/data/parsed_raw

ORDER=order.csv
AWK_SCRIPT=extract_parse.awk

#set -e # Exit if any command fails

# To conserve space, allow raw data to be deleted if "--delete-raw" flag is set
if [[ $# -eq 1 && $1 == "--delete-raw" ]] ; then 
    DELETE_RAW=true
    echo "WARNING: Raw data will be deleted!"; 
fi

# Remove previous version of files about to be produced
# Create output directories
rm -f log.txt
rm -f -r $EXTRACT_OUT_DIR
mkdir $EXTRACT_OUT_DIR
if [[ $PARSE_RAW -gt 0 ]] ; then
    rm -f -r $PARSE_OUT_DIR
    mkdir $PARSE_OUT_DIR
fi

# Loop through all test subject directories with valid data
IFS=$(echo -en "\n\b")
sub=$FIRST_SUBJECT
while [ -d "${IN_DIR}/surfaceV/S${sub}" ] ; do
    echo
    echo "                    Subject $sub"
    echo "----------------------------------------------------"

    # Build an array of all data files (sorted by version) for subject on SurfaceV
    fileArrayV=($(find ${IN_DIR}/surfaceV/S${sub} -type f -iregex .*\/S.*\.dat | sort -V))
    lengthV=${#fileArrayV[@]}

    # Build an array of all data files (sorted by version) for subject on SurfaceH
    fileArrayH=($(find ${IN_DIR}/surfaceH/S${sub} -type f -iregex .*\/S.*\.dat | sort -V))
    lengthH=${#fileArrayH[@]}

    if [ $lengthV -ne $lengthH ] ; then
        echo "Error: Subect #${sub}: Unequal data between SurfaceV and SurfaceH"
        exit 1
    fi

    # Read Order.csv to assign movement numbers
    mvtArray=($(gawk -F ',' 'NR > 2 { print $1 }' "${IN_DIR}/surfaceV/S${sub}/${ORDER}"))

    # Rename files to include Movement and Subject numbers
    for (( i=0; i<${lengthV}; i++ )) ; do

        # Extract node data into separate files
        gawk -f "$AWK_SCRIPT" -v period=$PERIOD -v overlap=$OVERLAP \
            -v surface="V" -v subject=$sub -v movement=${mvtArray[$i]} \
            -v num_lines=$(wc -l < "${fileArrayV[$i]}") \
            -v extract_outdir="${EXTRACT_OUT_DIR}" -v parse_outdir="${PARSE_OUT_DIR}" \
            -v doParse=$PARSE_RAW "${fileArrayV[$i]}"

        gawk -f "$AWK_SCRIPT" -v period=$PERIOD -v overlap=$OVERLAP \
            -v surface="H" -v subject=$sub -v movement=${mvtArray[$i]} \
            -v num_lines=$(wc -l < "${fileArrayH[$i]}") \
            -v extract_outdir="${EXTRACT_OUT_DIR}" -v parse_outdir="${PARSE_OUT_DIR}" \
            -v doParse=$PARSE_RAW "${fileArrayH[$i]}"

        # If the flag is set, delete data to conserve disk space
        if [[ $DELETE_RAW = true ]] ; then
            echo "Deleting: ${fileArrayV[$i]}"
            echo "Deleting: ${fileArrayH[$i]}"
            rm ${fileArrayV[$i]} ${fileArrayH[$i]}
        fi
    done

    echo
    echo "----------------------------------------------------"
    ((sub++))

    #exit 0   # Single subject only

done
echo
