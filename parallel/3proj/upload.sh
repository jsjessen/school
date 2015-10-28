#!/bin/bash

USERNAME='jjessen'
PACKAGE='package'

set -e # exit if anything fails

echo
echo "Compressing..."
zip -r ${PACKAGE}.zip $* 

echo "Uploading..."
scp ${PACKAGE}.zip ${USERNAME}@ssh1.eecs.wsu.edu:~/${PACKAGE}.zip
