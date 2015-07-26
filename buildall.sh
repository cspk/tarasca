#!/bin/bash

cd `dirname $0`
make clean
./configure
make generate_graph
make clean
./configure --cc=arm-linux-gnueabi-gcc

make all


