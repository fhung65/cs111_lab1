#!/bin/bash
rm log.txt
touch log.txt

mkdir diffs
for each in `ls case | grep .*.sh | grep -v io`
do
	( case/$each ; echo $? ) > "diffs/$each.res.bash.tmp"
	../profsh case/$each > "diffs/$each.res.profsh.tmp"
	echo $each differences ----------------- #>>log.txt
	diff "diffs/$each.res.bash.tmp" "diffs/$each.res.profsh.tmp" #>>log.txt
	rm "diffs/$each.res.bash.tmp" "diffs/$each.res.profsh.tmp"
done
