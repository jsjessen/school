#!/bin/bash

USERNAME='wsu483ak'
OUTFILE=$1

scp ${USERNAME}@users.isi.deterlab.net:/users/${USERNAME}/${OUTFILE} .
