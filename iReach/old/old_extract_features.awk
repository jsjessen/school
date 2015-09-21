#!/usr/local/bin/gawk

# James Jessen
# Project iReach
# August 2015 

# 20 Test Subjects that wore 10 Nodes and performed 26 movements
# Each Node has 4 sensor types with 3 dimensions

# INPUT VARIABLES: int period, int overlap, string surface, int subject, int movement, int num_lines, string outdir

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

function getOutfile(surface, node, outdir, movement, subject, nodes_per_file,   disp_node)
{
    if (surface == "V")
        disp_node = 1 + node
    else
        disp_node = 1 + node + nodes_per_file 

    return outdir "/" "sub" subject "-mvt" movement "-node" disp_node ".csv"
}

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

#******************************************************************************** 

BEGIN { 

    # For both input/output, fields are seperated by tabs and records by newlines
    FS  = "\t"
    RS  = "\n"
    OFS = "," 
    ORS = "\n"

    # Each file contains data for 5 of the 10 Nodes 
    nodes_per_file = 5

    # Use RAW or CAL data
    use_raw_data = 1
    use_cal_data = 2

    # Only useful header is on Row 2
    # Data starts on Row 5
    header_row = 2
    data_start_row = 5
    last_row = num_lines - data_start_row

    # Last 2 columns has garbage data that should be ignored
    width_end_garbage = 2

    # 4 Sensor types
    num_sensor_types = 4
    num_dimensions = 3
    # Each with 3 dimension and RAW / CAL data
    data_fields_per_node = num_sensor_types * num_dimensions * 2

    # Statistics
    num_stats = 9
    stat["label"][0] = "Max" 
    stat["label"][1] = "Min" 
    stat["label"][2] = "Range" 
    stat["label"][3] = "Mean" 
    stat["label"][4] = "Median" 
    stat["label"][5] = "Amplitude" 
    stat["label"][6] = "Variance" 
    stat["label"][7] = "Standard Deviation" 
    stat["label"][8] = "RMS" 

    size = 1
    buf_index = 0
    bufferFull = 0
}

#******************************************************************************** 

FNR == header_row {

    # Determine field arrangement because it is not consistent over all of the data files
    fields_per_node = (NF - width_end_garbage) / nodes_per_file
    data_start_field = (fields_per_node - data_fields_per_node) + use_cal_data

    # For each Node
    for(node = 0; node < nodes_per_file; node++)
    {
        outfile = getOutfile(surface, node, outdir, movement, subject, nodes_per_file)

        # For each useful field within this Node
        offset = node * fields_per_node
        for(field = data_start_field + offset; field <= offset + fields_per_node; field += 2)
        {
            printf "%s", $field > outfile  
            for(i = 0; i < num_stats; i++)
                printf "%s", OFS > outfile  
        }
        printf "%s", ORS > outfile # Next row

        for(sensor = 0; sensor < num_sensor_types * num_dimensions; sensor++)
        {
            for(label = 0; label < num_stats; label++)
                printf "%s%s", stat["label"][label], OFS > outfile  
        }
        printf "%s", ORS > outfile # Next row
    }
}

#******************************************************************************** 

# Process the data and headers
FNR >= data_start_row {

    record = (FNR - data_start_row) + 1

    buf_index++
    if(buf_index > period)
        buf_index = 1

    if(size >= period || record == last_row)
        bufferFull = 1

    # For each Node
    for(node = 0; node < nodes_per_file; node++)
    { 
        outfile = getOutfile(surface, node, outdir, movement, subject, nodes_per_file)

        # For each useful field within this Node
        offset = node * fields_per_node
        for(field = data_start_field + offset; field <= offset + fields_per_node; field += 2)
        {
            # Store the data in an array for statistical analysis
            buf[field][buf_index] = $field

            # If the array is filled with a period's worth of data
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
                for(value = 0; value < num_stats; value++)
                    printf "%s%s", stat["value"][value], OFS > outfile  

                if(record + (period - overlap) == last_row)
                {
                    i = buf_index + 1
                    while(1)
                    {
                        i = (i % period)
                        if(i == (buf_index + (period - overlap)) % period)
                            break
                        delete buf[field][i]
                        i++
                    }
                }
                delete sorted
                delete stat["value"]
            }
        }

        if(bufferFull)
            printf "%s", ORS > outfile # Next row
    }

    if(bufferFull)
    {
        bufferFull = 0
        size = period - overlap
    }
    else
        size++
}
