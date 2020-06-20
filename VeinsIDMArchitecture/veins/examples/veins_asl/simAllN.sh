#!/bin/bash
maxN=$1
echo "Simulating for 1 vehicle."
./change_N.sh 1
for N in $(seq 10 10 $maxN)
do
	echo "Simulating for $N vehicles."
	./change_N.sh $N
done
