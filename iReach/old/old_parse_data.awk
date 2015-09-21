#!/usr/local/bin/gawk

# James Jessen
# Project iReach
# August 2015 

# 20 Test Subjects that wore 10 node_fields and performed 26 movements
# Each Node has 4 sensor types with 3 dimensions

# INPUT VARIABLES: int period, int overlap, string surface, int subject, int movement, int num_lines, string outdir

# INPUT FILE: Data file from Surface V or H, for some Subject performing some movement

# 1  | ----------------------------------- Node 1 -------------------------------- | Node 2 | Node 3 | Node 4 | Node 5 | xxxxxxxxx | xxxxxxxxx |
# 2  | xxxxxxx | ... |---- Low Accel XYZ ----| Gyro XYZ | Wide Accel XYZ | Mag XYZ |  ...   |  ...   |  ...   |  ...   | xxxxxxxxx | xxxxxxxxx |
# 3  | xxxxxxx | ... |RAW|CAL|RAW|CAL|RAW|CAL|   ...    |      ...       |   ...   |  ...   |  ...   |  ...   |  ...   | xxxxxxxxx | xxxxxxxxx |
# 4  | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | xxxxxxxxx | xxxxxxxxx |
# 5+ | ------------------------------------------------------- Data -------------------------------------------------- |    ...    |    ...    |

BEGIN { 
    # For both input/output, fields are seperated by tabs and records by newlines
    FS  = "\t"
    RS  = "\n"
    OFS = "\t" 
    ORS = "\n"

    # Ignore case when comparing fields to regex
    IGNORECASE = 1

    # Important rows
    node_row = 1
    sensor_row = 2
    format_row = 3
    data_start_row = 5

    #last_row = num_lines - data_start_row

    num_nodes = 10
    if(surface == "V")
        initial_node = 1
    else
        initial_node = 6

    partial_outfile = outdir "/" "sub" subject "-mvt" movement "-node"

    fileBadFlag = 0
}

#******************************************************************************** 

# 0) Be able to determine which node any given field belongs to
FNR == node_row {
    # Keep track of the name of the current node to determine when nodes change
    node = initial_node
    cur_node = $1 

    for(field = 1; field <= NF; field++)
    {
        if($field ~ /Head|Chest|Backpack|Arm|Thigh|Ankle|Wrist|Waist/)
        {
            if($field != cur_node)
            {
                cur_node = $field
                node++
            }
            node_fields[field] = node
        }
    }
}

# 1) Read header from row2, filling an array with column numbers that contain one of the sensor names
FNR == sensor_row {
    for(field = 1; field <= NF; field++)
    {
        if($field ~ /(((Low Noise|Wide Range) Accelerometer)|Gyroscope|Magnetometer) [XYZ]/)
            sensor_fields[field] = $field 
    }
}

# 2) Read header from row3, filling an array with column numbers that contain "CAL"
FNR == format_row {
    for(field = 1; field <= NF; field++)
    {
        if(field in sensor_fields && $field ~ /CAL/)
        {
            valid_fields[field] = 1
            printf "%s%s", sensor_fields[field], OFS > partial_outfile node_fields[field] ".dat"
        }
    }

    # Move to the next row in each node's output file
    for(node = initial_node; node <= num_nodes; node++)
        printf "%s", ORS > partial_outfile node ".dat"
}

# Print to file whose name is based on Subject, Movement, Node 
# But only fields that are in both Sensor and CAL arrays
FNR >= data_start_row {
    rowBadFlag = 0
    for(field = 1; field <= NF; field++)
    {
        if(field in valid_fields)
        {
            printf "%s%s", $field, OFS > partial_outfile node_fields[field] ".dat"

            # Alert user to check for file corruption if there are empty data fields
            # This could be improved by using regex to check for time format data
            if(!($field))
                rowBadFlag = 1
        }
    }

    # Move to the next row in each node's output file
    for(node = initial_node; node <= num_nodes; node++)
        printf "%s", ORS > partial_outfile node ".dat"

    if(rowBadFlag)
    {
        fileBadFlag = 1
        print "WARNING: Bad raw data - Surface" surface " Subject" subject " Movement" movement > "./log.txt"
    }

    END {
        if(!fileBadFlag)
            printf "%s", "."
        else
            printf "%s", "X"
    }
