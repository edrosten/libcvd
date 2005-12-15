#!/bin/bash
for i in `cat .sourcefiles`
do 
	echo -n ${i%/*}/
	g++ $1 -MM -MG -DMAKE_DEPEND $i
done
