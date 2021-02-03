#!/bin/bash

TOPDIR=`pwd`
BINDIR=${TOPDIR}/build/demo
CONFIG=${TOPDIR}/test/config.txt

xterm -T "Client" -e "/bin/bash -c 'cd ${BINDIR}/client  && ./client -D mnist -M HCNN-DA -C ${CONFIG}; exec /bin/bash -i'"&
xterm -T "Server" -e "/bin/bash -c 'cd ${BINDIR}/server  && ./server;                                  exec /bin/bash -i'"&
