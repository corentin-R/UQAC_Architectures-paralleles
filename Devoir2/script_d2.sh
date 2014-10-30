#!/bin/bash

for (( c=2; c<2*8388608+1; c*=2 ))
do
    echo  -e "$c \t"    `./d2s $c`	 "\t"   `./d2p $c`	
   
done 
exit 0