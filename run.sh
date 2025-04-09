#!/bin/bash

make clean
make
./output/hoss examples/example_w_bjt.hsp
cat ./output/netlist.cir
