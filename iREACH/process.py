#!/usr/bin/env python3
# Python 3.4.0
# Linux

# James Jessen
# Project iREACH

DEBUG = False

#-------------------------------------------------------------------------------

import os
import sys
import csv
import re
import datetime
import timeit
from operator import itemgetter

#************************
WINDOW = 200
OVERLAP = 50
DATA_GAP_LIMIT = 100

LEAVE_DATA_GAPS = True
#************************

#HOME_DIR = '/home/jsjessen/Downloads/iReach/data/'
HOME_DIR = '/media/jsj/DROPSHIP/iReach/data/'
RAW_DIR = HOME_DIR + 'raw/'

# SurfaceV collected data for nodes 1-5 and SurfaceH nodes 6-10
SURFACES = ('surfaceV', 'surfaceH')
INITIAL_NODE = {}
INITIAL_NODE['surfaceV'] = 1
INITIAL_NODE['surfaceH'] = 6

# RegEx to match fields in data files
DATA_FILE_PATTERN = re.compile('S\d+ - \d+-\d+-\d+[AP]M \d+-\d+-\d+\.dat')
NODE_PATTERN = re.compile('Head|Chest|Backpack|Arm|Thigh|Ankle|Wrist|Waist', re.IGNORECASE)
SENSOR_PATTERN = re.compile('(((Low Noise|Wide Range) Accelerometer)|Gyroscope|Magnetometer).*[XYZ]', re.IGNORECASE)
FORMAT_PATTERN = re.compile('CAL', re.IGNORECASE)

# IO files
MOVEMENT_ORDER_FILE = 'order.csv'
OUTPUT_FILE = HOME_DIR + 'processed.csv'
LOG_FILE = HOME_DIR + 'processing_log.csv'

# Rows of interest
HEADER_NODE_ROW = 1
HEADER_SENSOR_ROW = 2
HEADER_FORMAT_ROW = 3
DATA_START_ROW = 5

# Node info
NODES_PER_FILE = 5

# Sensor info
NUM_SENSOR_TYPES = 4
NUM_DIMENSIONS = 3

# Headers
NUM_FIELDS_PER_FILE = NODES_PER_FILE * NUM_SENSOR_TYPES * NUM_DIMENSIONS
NUM_FIELDS_PER_NODE = NUM_SENSOR_TYPES * NUM_DIMENSIONS

NUM_MOVEMENTS = 24

#===============================================================================

def print_error(*objs):
    print("ERROR:", *objs, file=sys.stderr)
    sys.exit()

#-------------------------------------------------------------------------------

def print_warning(log, logger):
    logger.writerow(log)

#-------------------------------------------------------------------------------

# For display purposes only
def print_start_seperator(subject):
    print()
    print('                    Subject', subject)
    print('-' * NUM_MOVEMENTS * len(SURFACES))

#-------------------------------------------------------------------------------

# For display purposes only
def print_end_seperator(startTime):
    print('\n' + '-' * NUM_MOVEMENTS * len(SURFACES))
    endTime = timeit.default_timer()
    print('Time:', round(endTime - startTime), 'sec')

    if DEBUG:
        endTime = timeit.default_timer()
        totalTime = endTime - startTime
        if totalTime > 10:
            sys.exit()

#-------------------------------------------------------------------------------

# In the future it would be helpful if the surface files where within the subject
# directories, otherwise the surfaces must be looped through within each level.
# This can easily be accomplished by utilizing a cloud data service.
def process_data(startTime):
    """Process data by subject->movement->surface->node"""
    with open(OUTPUT_FILE, 'w') as outfile, open(LOG_FILE, 'w') as logfile:
        writer = csv.writer(outfile, delimiter=',')
        logger = csv.writer(logfile, delimiter=',')
        logger.writerow(['Description', 'Surface', 'File', 'Start', 'End', 'Size'])

        # Get sorted subject directories
        subjects = {}
        for surface in SURFACES:
            subjects[surface] = order_subject_dirs(os.path.join(RAW_DIR, surface))
        if subjects[SURFACES[0]] != subjects[SURFACES[1]]:
            print_error('Subject folders to not match between surfaces')

        # For each subject
        for subject in subjects[SURFACES[0]]:
            print_start_seperator(subject)

            # Read the movement order file from surfaceV
            # Determine how the data files map to movement numbers
            # Get an ordered list of movements and file associations
            movements = get_col_array(os.path.join(RAW_DIR, SURFACES[0], \
                                      subject, MOVEMENT_ORDER_FILE), 2, 0)
            numMovements = len(movements)
            for i in range(numMovements):
                movements[i]= {'mvt_num': movements[i], 'file_index': i}
            movements = sorted(movements, key=itemgetter('mvt_num'))

            # Get a sorted list of the data files in a subject's directory
            dataFiles = {}
            for surface in SURFACES:
                sub_dir = os.path.join(RAW_DIR, surface, subject)
                dataFiles[surface] = get_datafiles(subject, sub_dir, logger)
                if len(dataFiles[surface]) > numMovements:
                    print_error('Cannot determine movement order:', surface, subject)
            if len(dataFiles[SURFACES[0]]) != len(dataFiles[SURFACES[1]]):
                print_error('Unequal number of data files between surfaces:', \
                                                            surface, subject)
            # For each movement
            for movement in movements:
                # For each surface
                for surface in SURFACES:
                    mvt = {}
                    try:
                        mvt['file'] = dataFiles[surface][movement['file_index']]
                        mvt['num'] = movement['mvt_num']
                    except:
                        #print_warning(['No data file associated with movement', \
                        #                surface, mvt['num']], logger)
                        continue
                    sub_dir = os.path.join(RAW_DIR, surface, subject)
                    with open(os.path.join(sub_dir, mvt['file']), 'r') as infile:
                        reader = list(csv.reader(infile, delimiter='\t'))
                        process_file(reader, writer, logger, surface, subject, mvt)

                    # Print dot for each file processed to indicate progress.
                    print('.', end="", flush=True)

            # After all of the movements have been processed for a subject.
            print_end_seperator(startTime)
            startTime = timeit.default_timer()

#-------------------------------------------------------------------------------

def process_file(reader, writer, logger, surface, subject, movement):
    numFields = len(reader[0])

    # Figure out which columns are important and should be kept
    valid_fields = get_valid_fields(reader, logger, INITIAL_NODE[surface], numFields)
    check_fields(valid_fields, surface, movement['file'], writer)

    gap = []
    dataGaps = []
    node_data = []
    init_node = INITIAL_NODE[surface]
    time = 0
    # Loop through row by row
    for row in range(DATA_START_ROW - 1, len(reader)):
        outrow = []
        # Check for gaps in the data
        try:
            for field in sorted(valid_fields):
                outrow.append(reader[row][field])
        except:
            if not gap:
                gap.append(row)
            if gap[-1] != row:
                if gap[-1] + 1 != row:
                    dataGaps.append(gap)
                    gap = []
                gap.append(row)
            if LEAVE_DATA_GAPS:
                outrow = [None] * NUM_FIELDS_PER_FILE
            else:
                continue

        # Add subject, movement, node, and time fields to the end
        outrows = list(segment_list(outrow, NUM_FIELDS_PER_NODE))
        for node in range(NODES_PER_FILE):
            outrows[node].append(re.findall('\d+', subject)[0])
            outrows[node].append(movement['num'])
            outrows[node].append(node + init_node)
            outrows[node].append(time)
        node_data.append(outrows)
        time += 1

    # Log any data gaps
    for gap in dataGaps:
        gapStart = gap[0]
        gapEnd = gap[-1]
        gapSize = (gapEnd - gapStart) + 1
        print_warning(['Data Gap', surface, movement['file'], \
                        gapStart, gapEnd, gapSize], logger)

    # Write the desired data to the output file
    for node in range(NODES_PER_FILE):
        for row in node_data:
            writer.writerow(row[node])

#-------------------------------------------------------------------------------

def get_col_array(csvfile, startRow, col):
    """Return column from csv file as array, starting at specified row"""
    with open(csvfile, 'r') as infile:
        reader = csv.reader(infile)
        for i in range(startRow):
            next(reader)
        array = []
        for row in reader:
            array.append(int(row[col]))
        return array

#-------------------------------------------------------------------------------

def numerical_sort(value):
    """Extract the numbers from a string for numeric sorting"""
    numbers = re.compile('(\d+)')
    parts = numbers.split(value)
    parts[1::2] = map(int, parts[1::2])
    return parts

#-------------------------------------------------------------------------------

def order_subject_dirs(surface_dir):
    """Return a sorted list of subject directories"""
    return sorted([ s for s in os.listdir(surface_dir) \
                        if os.path.isdir(os.path.join(surface_dir, s)) ], \
                        key=numerical_sort)

#-------------------------------------------------------------------------------

def get_datafiles(subject, sub_dir, logger):
    """Return a list of all data files (sorted by version) for subject on surface"""
    dataFiles = list(filter(lambda s: re.match(DATA_FILE_PATTERN, s), \
                            os.listdir(sub_dir)))
    dataFiles = sorted(dataFiles, key=lambda x: datetime.datetime.strptime(x, \
                        subject + ' - %m-%d-%Y%p %H-%M-%S.dat'))
    if not dataFiles:
        print_warning(list("Failed to detect data files!"), logger)
    return dataFiles

#-------------------------------------------------------------------------------

def get_valid_fields(reader, logger, start_node, numFields):
    valid_fields = {}
    node = start_node
    cur_node = reader[HEADER_NODE_ROW - 1][0]
    for field in range(numFields):
        node_val   = reader[HEADER_NODE_ROW - 1][field]
        sensor_val = reader[HEADER_SENSOR_ROW - 1][field]
        format_val = reader[HEADER_FORMAT_ROW - 1][field]
        if(NODE_PATTERN.search(node_val)
           and SENSOR_PATTERN.search(sensor_val)
           and FORMAT_PATTERN.search(format_val)):
            if(node_val != cur_node):
                cur_node = node_val
                node += 1
            valid_fields[field] = (node, node_val, sensor_val, format_val)
    return valid_fields

#-------------------------------------------------------------------------------

def check_fields(valid_fields, surface, dataFile, writer):
    if not valid_fields:
        print_error('No valid fields:', surface, dataFile)
    if len(valid_fields) < NUM_FIELDS_PER_FILE:
        print_error('Too few headers:', surface, dataFile)
    if len(valid_fields) > NUM_FIELDS_PER_FILE:
        print_error('Too many headers:', surface, dataFile)
    if check_fields.header:
        for field, i in zip(sorted(valid_fields), range(NUM_FIELDS_PER_FILE)):
            if valid_fields[field][2] != check_fields.header[i % NUM_FIELDS_PER_NODE]:
                print_error("Headers don't match output file's:", surface, dataFile)
    else:
        outrow = []
        for field, i in zip(sorted(valid_fields), range(NUM_FIELDS_PER_FILE)):
            if i == NUM_FIELDS_PER_NODE:
                break
            outrow.append(valid_fields[field][2])
        outrow.append('Subject')
        outrow.append('Movement')
        outrow.append('Node')
        outrow.append('Time')
        writer.writerow(outrow)
        check_fields.header = outrow
check_fields.header = []

#-------------------------------------------------------------------------------

def segment_list(mylist, seg_size):
    for i in range(0, len(mylist), seg_size):
        yield mylist[i : i + seg_size]

#================================= Main ========================================

if __name__ == '__main__':
    startTime = timeit.default_timer()
    process_data(startTime)
    endTime = timeit.default_timer()

    print('Total Time:', round(endTime - startTime), 'sec')
