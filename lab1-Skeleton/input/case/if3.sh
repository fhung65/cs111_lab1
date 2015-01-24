if 
	if test 1 -eq 1
	then
		test 1 -eq 1
	else 
		test 1 -eq 4
	fi
then
	if test 4 -lt 3
	then
		echo yay
	else
		echo nay
	fi
else
	echo booh hoo!
fi
