#!/bin/bash

#pour chaque dim-linuxmpi
for (( c=1; c<6+1; c++ ))
do
	echo -e "linuxmpi $c "
	#on copit les sources sur la machine distante
	scp devoir5.c  8inf856-16@dim-linuxmpi$c.uqac.ca:/home/users/8inf856-16/Devoir5/
	#on éxécute le scipt qui le comilera les sources
	ssh 8inf856-16@dim-linuxmpi$c "bash -s" < compile_one.sh
	echo -e " "
done

ssh 8inf856-16@dim-linuxmpi1 "bash -s" < execute.sh


exit 0