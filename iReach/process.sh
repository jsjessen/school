#!/bin/bash

# James Jessen
# Project iREACH
# August 2015 

# This script will extract statistical features from the raw data
# and output it into well named files of the form: sub#-mvt#-node#

# If this var is true, parsed raw data will be similarly outputted as well 
PARSE_RAW=1

FIRST_SUBJECT=3  # First subject with valid data

WINDOW=200
OVERLAP=50

# If the data gap is larger than this, the data file will be discarded
DATA_GAP_LIMIT=100 

#set -e # Exit if any command fails

#IN_DIR=./data/raw
#PARSE_OUT_DIR=./data/parsed_raw
#EXTRACT_OUT_DIR=./data/features-window${WINDOW}-overlap${OVERLAP}

IN_DIR=/media/jsj/DROPSHIP/data/raw
PARSE_OUT_DIR=/media/jsj/DROPSHIP/data/parsed_raw
EXTRACT_OUT_DIR=/media/jsj/DROPSHIP/data/features-window${WINDOW}-overlap${OVERLAP}

ORDER=order.csv
AWK_SCRIPT=extract_parse.awk

#==============================================================================

# Remove previous version of files about to be produced
# Create output directories
rm -f -r $EXTRACT_OUT_DIR
mkdir $EXTRACT_OUT_DIR
if [[ $PARSE_RAW -gt 0 ]] ; then
    rm -f -r $PARSE_OUT_DIR
    mkdir $PARSE_OUT_DIR
fi

rm -f data_gap_log.csv
printf 'Surface,Subject,Movement,Start Line,End Line,Size\n' > data_gap_log.csv

IFS=$(echo -en "\n\b")
sub=$FIRST_SUBJECT
# For each Subject
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
        echo "Error: Unequal number of movements for SurfaceV-S${sub} and SurfaceH-S${sub}" 
        exit 1
    fi

    # Read order.csv to assign movement numbers
    mvtArray=($(gawk -F ',' 'NR > 2 { print $1 }' "${IN_DIR}/surfaceV/S${sub}/${ORDER}"))

    # For each Movement
    for (( i=0; i<${lengthV}; i++ )) ; do
        large_data_gap=0

        # Process and extract raw data from surfaceV
        gawk -f "$AWK_SCRIPT" \
            -v surface="V" -v subject=$sub -v movement=${mvtArray[$i]} \
            -v window=$WINDOW -v overlap=$OVERLAP -v data_gap_limit=$DATA_GAP_LIMIT \
            -v extract_outdir="${EXTRACT_OUT_DIR}" -v parse_outdir="${PARSE_OUT_DIR}" \
            -v num_lines=$(wc -l < "${fileArrayV[$i]}") -v doParse=$PARSE_RAW \
            "${fileArrayV[$i]}"

        # If there were problems with the data, remove related files
        if [ "$?" -gt 0 ]; then
            rm --verbose "${PARSE_OUT_DIR}/sub${sub}-mvt${mvtArray[$i]}"*
            rm --verbose "${EXTRACT_OUT_DIR}/sub${sub}-mvt${mvtArray[$i]}"*
            continue
        fi

        # Process and extract raw data from surfaceH
        gawk -f "$AWK_SCRIPT" \
            -v surface="H" -v subject=$sub -v movement=${mvtArray[$i]} \
            -v window=$WINDOW -v overlap=$OVERLAP -v data_gap_limit=$DATA_GAP_LIMIT \
            -v extract_outdir="${EXTRACT_OUT_DIR}" -v parse_outdir="${PARSE_OUT_DIR}" \
            -v num_lines=$(wc -l < "${fileArrayH[$i]}") -v doParse=$PARSE_RAW \
            "${fileArrayH[$i]}"

        # If there were problems with the data, remove related files
        if [ "$?" -gt 0 ]; then
            rm --verbose "${PARSE_OUT_DIR}/sub${sub}-mvt${mvtArray[$i]}"*
            rm --verbose "${EXTRACT_OUT_DIR}/sub${sub}-mvt${mvtArray[$i]}"*
            continue
        fi
    done

    echo
    echo "----------------------------------------------------"
    ((sub++))

    #exit 0   # Single subject only

done
echo
