#!/bin/bash

cd `dirname $0`
make clean
./configure
make -j `grep -c processor /proc/cpuinfo` generate_graph
make clean
./configure --cc=arm-linux-gnueabi-gcc

make -j `grep -c processor /proc/cpuinfo` all


