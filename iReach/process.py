#!/usr/bin/env python3
# Python 3.2.3
# Linux/Unix

# James Jessen
# iREACH

#-------------------------------------------------------------------------------

import os
import sys
import csv
import re
#import statistics
from distutils.version import StrictVersion

#WINDOW = 200
#OVERLAP = 50

#DATA_GAP_LIMIT = 100

RAW_DIR = '/media/jsj/DROPSHIP/data/raw'
PARSED_DIR = '/media/jsj/DROPSHIP/data'
#FEATURES_DIR = '/media/jsj/DROPSHIP/data/features-window${WINDOW}-overlap${OVERLAP}'

SURFACES = ('surfaceV', 'surfaceH')

DATA_FILE_PATTERN = '.*\/S.*\.dat'

MOVEMENT_ORDER_FILE = 'order.csv'
#LOG_FILE = open('log.csv', 'a')

#-------------------------------------------------------------------------------

#for sub in subjects:
#    for mvt in movements:
#        for node in nodes:
#            print
#        print
#    print

#-------------------------------------------------------------------------------

#
def get_col_array(csvfile, startRow, col):
    """Return column from csv file as array, starting at specified row"""
    with open(csvfile, 'r') as infile:
        reader = csv.reader(infile)
        for i in xrange(startRow - 1):
            next(reader)
        for row in reader:
            array = list(row[0])
        return array

numbers = re.compile(r'(\d+)')
def NumericalSort(value):
    parts = numbers.split(value)
    parts[1::2] = map(int, parts[1::2])
    return parts

#-------------------------------------------------------------------------------

def process_data():
    # For each Subject
    for surface in SURFACES:
        surface_dir = RAW_DIR + '/' + surface

        subjects = sorted(
                [ s for s in os.listdir(surface_dir)
                    if os.path.isdir(os.path.join(surface_dir, s)) ],
                key=NumericalSort)
#*** PASS ***

        for sub in subjects:
            sub_dir = surface_dir + subject

            print()
            print('           ' + surface + ' : Subject ' + sub)
            print('----------------------------------------------------')

            # Build a list of all data files (sorted by version) for subject on surface
            data_files = filter(lambda s: re.match(DATA_FILE_PATTERN, s), os.listdir(path))
            data_files.sort(key=StrictVersion) # sort so order corresponds to order.csv entries

            mvtOrder = get_mvt_order(path + '/' + MOVEMENT_ORDER_FILE, 2, 1)

            for dataFile in dataFiles:

                # Process and extract raw data from surface
                parse(data_file, surface, movement)

                print()
                print('----------------------------------------------------')

                #((sub++))

               #exit 0   # Single subject only

#================================= Main ========================================

if __name__ == '__main__':
    process_data()
