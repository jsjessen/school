#!/usr/local/bin/gawk

# James Jessen
# Project iREACH
# August 2015 

# 20 Test Subjects that wore 10 Nodes and performed 26 movements
# Each Node has 4 sensor types with 3 dimensions

# INPUT VARIABLES: int window, int overlap, string surface, int subject, int movement, int num_lines, string outdir

# INPUT FILE: Data file from Surface V or H, for some Subject performing some movement

# 1  | ----------------------------------- Node 1 -------------------------------- | Node 2 | Node 3 | Node 4 | Node 5 | xxxxxxxxx | xxxxxxxxx |
# 2  | xxxxxxx | ... |---- Low Accel XYZ ----| Gyro XYZ | Wide Accel XYZ | Mag XYZ |  ...   |  ...   |  ...   |  ...   | xxxxxxxxx | xxxxxxxxx |
# 3  | xxxxxxx | ... |RAW|CAL|RAW|CAL|RAW|CAL|   ...    |      ...       |   ...   |  ...   |  ...   |  ...   |  ...   | xxxxxxxxx | xxxxxxxxx |
# 4  | xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx | xxxxxxxxx | xxxxxxxxx |
# 5+ | ------------------------------------------------------- Data -------------------------------------------------- |    ...    |    ...    |

# OUTPUT FILE: Data file with statistical features for a specific subject, node, movement combination 

# 1  | ----------------------------------------- Low Accel X ---------------------------------------| Low Accel Y | Low Accel Z | Gyro X | ... |
# 2  | Rows | Max | Min | Range | Mean | Median | Amplitude | Variance | Standard | Deviation | RMS |    ...      |     ...     |   ...  | ... |
# 3+ | -------------------------------------------------------------- Data --------------------------------------------------------------------|

function sum(vec,   i, ret)
{
    for(i in vec)
        ret += vec[i]
    return ret
}

function mean(vec, n)
{
    return sum(vec) / n
}

function median(sorted, n)
{
    if(n % 2)
        return sorted[(n + 1) / 2]
    else
    {
        # For even sized sets, use the mean of the two middle elements
        return (sorted[(n / 2)] + sorted[(n / 2) + 1]) / 2.0
    }
}

function amp(max, mean)
{
    return max - mean
}

function var(vec, mean, n,   i, tmp)
{
    for(i in vec)
        tmp[i] = (vec[i] - mean) ^ 2

    # Divide by (n-1) for unbiased variance
    # Divide by n for biased sample variance
    # Divide by n for population variance
    return sum(tmp) / n
}

function std(var)
{
    return sqrt(var)
}

function rms(vec, n,   i, tmp)
{
    for(i in vec)
        tmp[i] = vec[i] ^ 2
    return sqrt(sum(tmp) / n)
}

#================================================================================ 

BEGIN { 
    # File input delimiters
    FS  = "[\t,]" # tab or comma delimited fields
    RS  = "\n"

    # File Output
    OFS = "," 
    ORS = "\n"
    partial_parse_outfile = parse_outdir "/sub" subject "-mvt" movement "-node"
    partial_extract_outfile = extract_outdir "/sub" subject "-mvt" movement "-node"
    out_file_type = ".csv"
    logfile = "./data_gap_log.txt"

    # for(index in array) now goes in order of numerically increasing index values
    PROCINFO["sorted_in"] = "@ind_num_asc" 

    # Ignore case when comparing field contents to regex
    IGNORECASE = 1

    # Rows of interest 
    node_row = 1
    sensor_row = 2
    format_row = 3
    data_start_row = 5

    # Node info
    nodes_per_file = 5

    # Sensor info
    num_sensor_types = 4
    num_dimensions = 3
    # Each with 2 data formats: | RAW | CAL |

    # Determine numeric node value based on 
    # source of raw data (surfaceV or surfaceH)
    if(surface == "V")
        initial_node = 1
    else
        initial_node = 6

    # Statistic Labels
    stat["label"][0] = "Max" 
    stat["label"][1] = "Min" 
    stat["label"][2] = "Range" 
    stat["label"][3] = "Mean" 
    stat["label"][4] = "Median" 
    stat["label"][5] = "Amplitude" 
    stat["label"][6] = "Variance" 
    stat["label"][7] = "Standard Deviation" 
    stat["label"][8] = "RMS" 

    # EOF
    last_row = num_lines - data_start_row

    # Initialize data read variables
    size = 1
    buf_index = 0
    bufferFull = 0

    # Initially no problems encountered with file read
    disp_symbol = "."
    count = data_start_row
}

#================================================================================ 

# Strategy: Figure out useful fields and loop/file information 
#           by finding header fields with a Node and Sensor and CAL

# 1) Read node header labels: build array matching field number to node
FNR == node_row {
    num_fields = NF
    if(num_fields < 140)
    {
        print "\nERROR: Incomplete Header: surface" surface "-sub" subject "-mvt" movement ": Fields = " NF "\n"
        exit 2
    }

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

# 2) Read sensor header labels: build array matching column number to sensor name
FNR == sensor_row && NF == num_fields {
    for(field = 1; field <= NF; field++)
    {
        if($field ~ /(((Low Noise|Wide Range) Accelerometer)|Gyroscope|Magnetometer) [XYZ]/)
            sensor_fields[field] = $field 
    }
}

# 3) Create an array of valid fields for each node by checking for Sensor and CAL
#    Write header labels to output files
FNR == format_row && NF == num_fields {
    # For each node create an array of valid fields
    for(field in sensor_fields)
    {
        if($field !~ /CAL/)
            continue

        node = node_fields[field]
        valid_fields[node][field] = 1

        parse_outfile = partial_parse_outfile node out_file_type

        if(doParse)
            printf "%s%s", sensor_fields[field], OFS > parse_outfile 
    }

    # For each node:
    for(node = initial_node; node < initial_node + nodes_per_file; node++)
    {
        parse_outfile = partial_parse_outfile node out_file_type
        extract_outfile = partial_extract_outfile node out_file_type

        # For each valid field in node:
        for(field in valid_fields[node])
        {
            # Print header labels
            printf "%s", sensor_fields[field] > extract_outfile

            # Make it so each header label has room for stats underneath it
            for(statistic in stat["label"])
                printf "%s", OFS > extract_outfile  
        }

        # Move to the next row, preparing for stat headers
        printf "%s", ORS > extract_outfile 

        # Print statistic header labels
        for(sensor = 0; sensor < num_sensor_types * num_dimensions; sensor++)
        {
            for(label in stat["label"])
                printf "%s%s", stat["label"][label], OFS > extract_outfile 
        }

        # Move to the next row, preparing for data
        printf "%s", ORS > extract_outfile
        if(doParse)
            printf "%s", ORS > parse_outfile 
    }
}

# 4) Print data from valid fields to output files,
#    performing calculations as necessary
FNR >= data_start_row && NF == num_fields {
    if(count < FNR)
    {
        disp_symbol = "!"
        data_gap = FNR - count
        printf "%s%s", surface, OFS >> logfile
        printf "%d%s", subject, OFS >> logfile
        printf "%d%s", movement, OFS >> logfile
        printf "%d%s", count, OFS >> logfile
        printf "%d%s", FNR-1, OFS >> logfile
        printf "%d%s", data_gap, ORS >> logfile

        if(data_gap > data_gap_limit)
        {
            printf "%s", "D"
            exit 1 # process.sh will delete the appropriate files
        }
    }
    count++

    record = (FNR - data_start_row) + 1

    buf_index++
    if(buf_index > window)
        buf_index = 1

    if(size >= window || record == last_row)
        bufferFull = 1

    # For each Node
    for(node = initial_node; node < initial_node + nodes_per_file; node++)
    { 
        extract_outfile = partial_extract_outfile node out_file_type
        parse_outfile = partial_parse_outfile node out_file_type

        # For each valid field in node:
        for(field in valid_fields[node])
        {
            # Store the data in an array for statistical analysis
            buf[field][buf_index] = $field

            if(doParse)
                printf "%s%s", $field, OFS > parse_outfile 

            # If the array is filled with a window's worth of data
            # or if it is the remainder of the file's data
            if(bufferFull)
            {
                asort(buf[field], sorted, "@val_num_asc")

                Max = sorted[size]
                Min = sorted[1]
                Mean = mean(sorted, size)
                Var = var(sorted, Mean, size) # Variance

                stat["value"][0] = Max                  # Max
                stat["value"][1] = Min                  # Min
                stat["value"][2] = Max - Min            # Range
                stat["value"][3] = Mean                 # Mean
                stat["value"][4] = median(sorted, size) # Median
                stat["value"][5] = amp(Max, Mean)       # Amplitude
                stat["value"][6] = Var                  # Variance
                stat["value"][7] = std(Var)             # Standard Deviation
                stat["value"][8] = rms(sorted, size)    # Root Mean Square

                # Print statistical data
                for(value in stat["value"])
                    printf "%s%s", stat["value"][value], OFS > extract_outfile  

                # If the buffer won't be filled because the last row is coming up
                # then delete the none overlap data from it
                if(record + (window - overlap) > last_row)
                {
                    i = buf_index + 1
                    while(1)
                    {
                        i = (i % window)
                        if(i == ((buf_index + window) - overlap + 1) % window)
                            break
                        else
                            delete buf[field][i++]
                    }
                }
                delete sorted
                delete stat["value"]
            }
        }# Field Loop

        # Move to the next row, preparing for more data
        if(bufferFull)
            printf "%s", ORS > extract_outfile
        if(doParse)
            printf "%s", ORS > parse_outfile 

    }# Node Loop

    # Update state for next row of data to be processed 
    if(bufferFull)
    {
        bufferFull = 0
        size = overlap
    }
    size++
}

END {
    printf "%s", disp_symbol
}
