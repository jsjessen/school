#!/bin/bash

PERIOD=500
OVERLAP=100

#IN_DIR=./data/raw
#OUT_DIR=./data/features-period${PERIOD}-overlap${OVERLAP}
IN_DIR=/media/jsj/DROPSHIP/data/raw
OUT_DIR=/media/jsj/DROPSHIP/data/features-period${PERIOD}-overlap${OVERLAP}

ORDER=Order.csv
FIRST_SUBJECT=3  # First subject with valid data

set -e # Exit if any command fails

# To conserve space, allow raw data to be deleted if "--delete-raw" flag is set
if [[ $# -eq 1 && $1 == "--delete-raw" ]] ; then 
    DELETE_RAW=true
    echo "WARNING: Raw data will be deleted!"; 
fi

# Fix permissions
#echo -n "Fixing file permissions..."
#sudo find $IN_DIR -type d -exec chmod 755 '{}' \;
#sudo find $IN_DIR -type f -exec chmod 644 '{}' \;
#echo "DONE"
#
# Convert Excel files to CSV format
#echo -n "Converting Excel files to CSV format..."
#find . -depth -name '*.xlsx' -exec \
#    sh -c 'ssconvert --export-type=Gnumeric_stf:stf_csv "$1" "${1%.xlsx}.csv" > /dev/null 2>&1' _ '{}' \;
#echo "DONE"
#
## Remove Excel files
#find . -depth -name '*.xlsx' -exec rm '{}' \;

#TEMP
#rm -f -r ${OUT_DIR}
#mkdir ${OUT_DIR}

# Loop through all test subject directories with valid data
IFS=$(echo -en "\n\b")
sub=$FIRST_SUBJECT
while [ -d "${IN_DIR}/surfaceV/S${sub}" ] ; do
    echo
    echo "        Subject $sub"
    echo "--------------------------"
    

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
        # Mark progress
        echo -n "."

        num_linesV=$(wc -l < "${fileArrayV[$i]}")
        num_linesH=$(wc -l < "${fileArrayH[$i]}")

        # Extract node data into seperate files
        gawk -f extract_data.awk -v period=$PERIOD -v overlap=$OVERLAP \
            -v surface="V" -v subject=$sub -v movement=${mvtArray[$i]} \
            -v num_lines=$num_linesV -v outdir="${OUT_DIR}" "${fileArrayV[$i]}"
        gawk -f extract_data.awk -v period=$PERIOD -v overlap=$OVERLAP \
            -v surface="H" -v subject=$sub -v movement=${mvtArray[$i]} \
            -v num_lines=$num_linesH -v outdir="${OUT_DIR}" "${fileArrayH[$i]}"

        # If the flag is set, delete data to conserve disk space
        if [[ $DELETE_RAW = true ]] ; then
            echo "Deleting: ${fileArrayV[$i]}"
            echo "Deleting: ${fileArrayH[$i]}"
            rm ${fileArrayV[$i]} ${fileArrayH[$i]}
        fi
    done

    echo
    echo "--------------------------"
    ((sub++))
done
echo
