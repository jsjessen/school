#!/bin/sh

IFACE='eth0'
USERNAME='wsu483ak'
OUTFILE='dump_out'

RUNTIME=$1

#timeout -k ${RUNTIME}s sudo tcpdump -i ${IFACE} -w /users/${USERNAME}/${OUTFILE}.pcap

sudo tcpdump -i ${IFACE} -w /users/${USERNAME}/${OUTFILE}.pcap & PID=$!
sleep $RUNTIME
sudo kill $PID
