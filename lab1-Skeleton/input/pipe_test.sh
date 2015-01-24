#!/bin/bash
start=2
end=18
num=$start

while test $num -le $end
do
	(./case/pipe$num.sh ; echo $? ) >res_a_$num.tmp
	../profsh ./case/pipe$num.sh >res_b_$num.tmp
	echo pipe$num.sh differences------------>>log.txt
	diff res_a_$num.tmp res_b_$num.tmp >>log.txt
	rm res_a_$num.tmp res_b_$num.tmp 
	let num+=1 
done
