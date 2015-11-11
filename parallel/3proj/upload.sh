#!/bin/bash

USERNAME='jjessen'
PACKAGE='package'
PROJECT='4proj'

set -e # exit if anything fails

echo
echo "Compressing..."
zip -r ${PACKAGE}.zip ${PROJECT}/$* 

echo "Uploading..."
scp ${PACKAGE}.zip ${USERNAME}@ssh1.eecs.wsu.edu:~/${PACKAGE}.zip
