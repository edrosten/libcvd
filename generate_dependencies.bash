#!/bin/bash
for i in `cat .sourcefiles`
do 
	echo -n ${i%/*}/
	$1 $2 -MM -MG -DMAKE_DEPEND $i
done
