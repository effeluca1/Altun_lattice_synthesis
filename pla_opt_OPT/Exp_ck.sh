#!/bin/sh
for i in $(ls *pla.lattice* | grep -v .dat | grep optimized)
do
    echo ${i%%.*}
	jstr=${i##*pla.lattice}
    echo ${jstr%%optimized}
HELLO=Hello

	#echo timeout 900 ./deftol $i $i $i $i ${i%%.*}.pla ${i##*pla.lattice}
	echo timeout 900 ./deftol $i $i $i $i ${i%%.*}.pla ${jstr%%optimized}
	     timeout 900 ./deftol $i $i $i $i ${i%%.*}.pla ${jstr%%optimized}

#&& echo mv *.eps check_graph/
done
