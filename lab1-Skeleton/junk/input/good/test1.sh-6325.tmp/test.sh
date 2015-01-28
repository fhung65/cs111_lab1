echo hi

echo one two three > 1.out

cat < 1.out

echo this; echo that

if cat 1.out;
then echo true;
else echo fail;
fi;

if cta;
then echo nothing;
else if cat;
then echo here;
fi<1.out;
fi>2.out;

cat 2.out

echo hi > tempfile

while cat tempfile;
do
rm tempfile; echo deleted;
done;

until cat tempfile;
do 
echo untiltest > tempfile
echo dountil;
done;

(
if ( test 1 -eq 1 )
then
(echo hello there! )
fi
)

(if test 1 -eq 1 ; then cat ; fi) <2.out

if test 1 -eq 1
then
	cat<1.out
fi <2.out

(if test 1 -eq 1 ; then echo out ; fi) >3.out

cat 3.out

(cat < 2.out | cat) < 3.out

echo hiya | cat < 3.out

echo hiya | cat

touch me

echo hey ; ls | grep me

rm me

if true
then cat < 3.out
fi | 
if false
then echo nope
else cat
fi |
if false
then echo stillnothing
else cat
fi

(echo hi | tac ) < 3.out > 4.out

cat 4.out

(echo never gonna give you up) | cat

echo never gonna let you down | (cat)

ls | grep .out | grep 2

echo never ; echo gonna; echo run; echo around;
echo and
echo desert
echo you

while false
do
echo this is bad
done

(cat < 1.out ; cat)< 3.out
