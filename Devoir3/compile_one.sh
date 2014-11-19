#!/bin/bash
#lamboot
cd TP3
echo "compilation"
mpicc -o prog.exe -fopenmp tp3.c
	



#echo "execution"
#./prog.exe

exit 0
