#!/bin/bash
for each in `ls case | grep .*sh` 
do
	echo $each
	../profsh "case/$each"
done
mv *.out out
rm *.tmp
