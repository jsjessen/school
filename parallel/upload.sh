#!/bin/bash

USERNAME='jjessen'
PACKAGE='package'
PROJECT='5proj'

set -e # exit if anything fails

echo
echo "Compressing..."
zip -r ${PACKAGE}.zip ${PROJECT}/*

echo "Uploading..."
scp ${PACKAGE}.zip ${USERNAME}@ssh1.eecs.wsu.edu:~/parallel_483/${PROJECT}/${PACKAGE}.zip
