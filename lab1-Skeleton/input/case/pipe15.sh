if test 1 -eq 1
then cat <in.txt
fi | 
if test 1 -eq 4
then echo naaay
else cat
fi | 
if test 1 -eq 5
then echo naaah
else cat 
fi
