#!/bin/bash
for each in `ls test | grep .*sh` 
do
	echo $each
	../profsh "test/$each"
done
mv *.out out
