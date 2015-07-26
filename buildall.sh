#!/bin/bash

cd `dirname $0`
./configure --cc=gcc
make generate_graph
make clean
./configure --cc=arm-linux-gnueabi-gcc
make all


