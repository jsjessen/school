#!/usr/bin/env python3
# Python 3.4.0
# Linux

# James Jessen
# Project iREACH

#-------------------------------------------------------------------------------

import os
import sys
import csv
import statistics
import math
import timeit

# Next time use this:
# wget https://3230d63b5fc54e62148e-c95ac804525aac4b6dba79b00b39d1d3.ssl.cf1.rackcdn.com/Anaconda3-2.3.0-Linux-x86_64.sh
# bash Anaconda3-2.3.0-Linux-x86_64.sh
# import pandas as pd
# import numpy as np

#  0     1      2      3       4          5          6         7        8
# Min | Max | Range | Mean | Median | Amplitude | Std Dev | Variance | RMS |

#************************
WINDOW = 200
OVERLAP = 50
DATA_GAP_LIMIT = 100
#************************

#HOME_DIR = '/home/jsjessen/Downloads/iReach/data/'
HOME_DIR = '/media/jsj/DROPSHIP/iReach/data/'

INPUT_FILE = 'processed.csv'
OUTPUT_FILE = 'extracted.csv'
LOG_FILE = 'extract_log.csv'

# Sensor info
NUM_SENSOR_TYPES = 4
NUM_DIMENSIONS = 3

# Headers
NUM_FIELDS_PER_NODE = NUM_SENSOR_TYPES * NUM_DIMENSIONS

NUM_FEATURES = 9

#===============================================================================

def print_error(*objs):
    print("ERROR:", *objs, file=sys.stderr)
    sys.exit()

#-------------------------------------------------------------------------------

def print_warning(log, logger):
    logger.writerow(log)

#-------------------------------------------------------------------------------

def print_outfile_header(writer):
    outrow = []
    for i in range(NUM_SENSOR_TYPES * NUM_DIMENSIONS * NUM_FEATURES):
        outrow.append('F' + str(i+1))
    outrow.append('Mote')
    writer.writerow(outrow)

#-------------------------------------------------------------------------------

def rms(arr):
    arr = list(map(lambda x: math.pow(x, 2), arr))
    return math.sqrt(math.fsum(arr) / float(len(arr)))

#-------------------------------------------------------------------------------

def get_stats(arr):
    min_ = min(arr)
    max_ = max(arr)
    range_ = max_ - min_
    mean_ = statistics.mean(arr)
    median_ = statistics.median(arr)
    amp_ = max_ - mean_
    try:
        stdev_ = statistics.stdev(arr)
        var_ = statistics.variance(arr)
    except:
        stdev_ = 0
        var_ = 0
    rms_ = rms(arr)

    result = []
    result.append(min_)
    result.append(max_)
    result.append(range_)
    result.append(mean_)
    result.append(median_)
    result.append(amp_)
    result.append(stdev_)
    result.append(var_)
    result.append(rms_)
    return result

#-------------------------------------------------------------------------------

def rotate_array(original):
    return list(zip(*original[::-1]))

#-------------------------------------------------------------------------------

def get_data(reader):
    """Read input file row by row until window filled or reach new section"""
    goalSize = WINDOW
    startNode = 1
    gap = 0 # Data gap within this window
    curOverlapGap = 0 # Data gap within the overlap used by this window
    nextOverlapGap = 0 # Data gap within the overlap that will be part of next window
    rowBuf = []
    # For each row
    for row in reader:
        size = len(rowBuf)
        endNode = int(row[-1])

        # Submit data
        if startNode != endNode or size + gap == goalSize:
            yield (startNode, rowBuf, gap + curOverlapGap, nextOverlapGap)
            del rowBuf[:]
            if startNode != endNode:
                # New Node
                goalSize = WINDOW
                curOverlapGap = 0
                # Print dot for each node to indicate progess
                print('.', end="", flush=True)
            else:
                # Same node
                goalSize = WINDOW - OVERLAP # Overlap will be added
                curOverlapGap = nextOverlapGap
            gap = 0
            nextOverlapGap = 0
            startNode = endNode

        fieldBuf = []
        # Read each field in the row and look for data gaps
        try:
            for field, width in zip(row, range(NUM_FIELDS_PER_NODE)):
                fieldBuf.append(float(field))
        except:
            gap += 1
            if size + gap > goalSize - OVERLAP:
                nextOverlapGap += 1
            continue
        rowBuf.append(fieldBuf)
    yield (startNode, rowBuf, gap + curOverlapGap, nextOverlapGap)

#-------------------------------------------------------------------------------

def get_features(data):
    features = []
    data = rotate_array(data)
    for row in data:
        features.extend(get_stats(row))
    return features

#-------------------------------------------------------------------------------

def extract_features(startTime):
    """Extract data by subject->movement->node"""
    with open(OUTPUT_FILE, 'w', newline='') as outfile, \
         open(os.path.join(HOME_DIR, INPUT_FILE), 'r', newline='') as infile:
        writer = csv.writer(outfile, delimiter=',')
        reader = csv.reader(infile, delimiter=',')

        next(reader) # skip over header
        print_outfile_header(writer)

        prevNode = -1
        overlapBuf = []
        for node, data, gap, nextOverlapGap in get_data(reader):
            if node == prevNode:
                data = overlapBuf + data
            prevNode = node
            overlapBuf = data[len(data) - OVERLAP + nextOverlapGap:]

            if gap > DATA_GAP_LIMIT:
                continue

            features = get_features(data)
            features.append(node)
            writer.writerow(features)

#================================= Main ========================================

if __name__ == '__main__':
    startTime = timeit.default_timer()
    extract_features(startTime)
    endTime = timeit.default_timer()

    print('Total Time:', round(endTime - startTime), 'sec')
