#!/bin/bash
rm log.txt
touch log.txt
mkdir diffs
for each in `ls case | grep .*.sh | grep -v io`
do
	( case/$each ; echo $? ) >diffs/$each_res_bash.tmp
	../profsh case/$each >diffs/$each_res_profsh.tmp
	echo $each differences -----------------
	diff diffs/$each_res_bash.tmp diffs/$each_res_profsh.tmp
	#rm $each_res_bash.tmp $each_res_profsh.tmp
done
