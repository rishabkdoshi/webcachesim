#!/bin/bash

cacheSize=$1
traceFile=$2
if [ "$1" == "" -o "$2" == "" ]; then
    echo "generate_results cacheSize traceFile"
    exit 1
fi

policies=(LRU FIFO GDS GDSF LFUDA LRUK AdaptSize)

for i in "${policies[@]}"
do
	echo "time ../webcachesim $2 $i $cacheSize"
	time ../webcachesim $2 $i $cacheSize
done
