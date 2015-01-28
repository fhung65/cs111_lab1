comment.sh
#

if1.sh
if test 1 -eq 1
then
echo yay
else
echo nay
fi

if2.sh
if test 1 -eq 5
then
echo yay
else
echo nay
fi

if3.sh
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

if4.sh
if test 1 -eq 1
then
	echo did things
fi

if5.sh
if test 2 -eq 123
then
	echo we have issues
else
	echo good
fi

in12.sh
#if test 1 -eq 1
#then
#	cat>in2.txt
#fi >in.txt

io10.sh
(if test 1 -eq 1 ; then cat ; fi) <in.txt

io11.sh
if test 1 -eq 1
then
	cat<in2.txt
fi <in.txt

io12.sh
if test 1 -eq 1
then
	cat<in2.txt
fi >out12.txt

io13.sh
(if test 1 -eq 1 ; then cat<in.txt ; fi) >out13.txt

io14.sh
(cat <in2.txt | cat)<in.txt

io15.sh
(cat | cat<in2.txt)<in.txt

io16.sh
(cat <in2.txt | cat)>out16.txt


io1.sh
echo hi >out1.out

io2.sh
echo howdy >out2.out ; 
cat <out2.out

io3.sh
echo this is a yappy flappy duck you schmuck! >input1.out
cat <input1.out >out3.out

io4.sh
echo zippidy doo da velicirapto, this is a new file >out4.out
echo nothing should follow this: >out4.out

io5.sh
(cat) <in.txt

io6.sh
if test 1 -eq 1 
then
	cat
fi <in.txt

io7.sh
until test 1 -eq 1
do
	cat
done <in.txt

io8.sh
(cat <in2.txt)<in.txt

io9.sh
(cat <in2.txt; cat)<in.txt

io_pipe1.sh
echo yargghghghgh >input2.txt
cat | grep <input2.txt 
# should execute grep with input2, and then cat with no input separately

io_pipe2.sh
( echo hey | cat ) <in.txt #should print out hey

io_pipe3.sh
cat <in.txt >out.txt | cat # outputs nothing

pipe10.sh
cat <in.txt | grep I

pipe11.sh
cat <in.txt | grep I >out_pipe.txt

pipe12.sh
cat in.txt >out_pipe12.txt | grep I

pipe13.sh
echo hiya | cat <in.txt  

pipe14.sh
if test 1 -eq 1
then
echo yoooooo
fi |
if test 1 -eq 1
then cat
fi

pipe15.sh
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

pipe16.sh
echo hiya ; echo yahaooda | cat ; echo haha

pipe17.sh
if test 1 -eq 1
then ls
fi |
if test 1 -eq 1
then grep sh
fi |
if test 1 -eq 3
then echo shell did bad things
else grep s
fi

pipe18.sh
if echo hah | grep h
then echo that worked
else echo never mind
fi

pipe19.sh
(echo hi | less) <in.txt

pipe20.sh
(echo hi | tac ) >out_pipe20.txt

pipe21.sh
if cat ; then grep I ; fi <in.txt

pipe22.sh
if ls ; then echo hi ; fi >output.txt

pipe2.sh
echo land ho! | if test 1 -eq 1 ; then cat ; fi

pipe3.sh
(echo haldo) | cat

pipe4.sh
echo haldo | (cat)

pipe5.sh
ls | grep sh | grep s

pipe6.sh
(ls | grep sh ) | grep s 

pipe7.sh
echo hey ; ls | grep pipe 

pipe8.sh
ls | grep sh ; echo dragoniyyite

pipe9.sh
ls | grep pipe | grep sh | grep s

sequence1.sh
echo bobby
echo yes

sequence2.sh
test 1 -lt 1
test 2 -eq 2
echo hi

sequence3.sh
echo hi ; echo bye ; test 3 -eq 1 ; 
echo moon ; test 2 -lt 3

simple1.sh
ls

simple2.sh
test 1 -lt 2

simple3.sh
echo hello there

stat1.sh
test 1 -eq 1

sub1.sh
( echo hi )

sub2.sh
( ( echo hi ) )

sub3.sh
(
	if ( test 1 -eq 1 )
	then
		( echo hello dere! )
	fi 
)

sub4.sh
( echo yes ) ; ( echo no )

( (test 1 -eq 1) ; (test 2 -eq 4) )

if ( ( test 3 -eq 1 ) ; ( test 5 -eq 5 ) )
then 
	echo hey!
else
	echo Nay!
fi



while1.sh
while false 
do
echo infiniting 
done >out5.out

while2.nosh
while test 1 -eq 1
do
	cat 
done < in.txt

