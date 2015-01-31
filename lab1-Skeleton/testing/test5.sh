if true
then
	if ( test 1 -eq 1 )
	then 
		sort ../main.c >file.txt
	fi
	rm file.txt
	#echo 1 | echo 2 | echo 3 | echo 4 | grep 4 ;
fi
