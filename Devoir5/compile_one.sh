#!/bin/bash
#
#lamboot
cd Devoir5
echo "compilation"
mpicc devoir5.c -lm -std=c99 -fopenmp -o devoir5

#echo "execution"
#./prog.exe

exit 0
