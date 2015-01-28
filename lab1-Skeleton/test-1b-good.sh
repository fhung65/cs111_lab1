#! /bin/sh

# UCLA CS 111 Lab 1 - Test cases for testing execution

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

tmp=$0-$$.tmp
mkdir "$tmp" || exit

(
cd "$tmp" || exit

cat >test.sh <<'EOF'
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
EOF

cat >test.exp <<'EOF'
hi
one two three
this
that
one two three
true
one two three
here
hi
deleted
dountil
untiltest
hello there!
one two three
here
one two three
out
one two three
here
out
hiya
hey
me
out
hi
never gonna give you up
never gonna let you down
2.out
never
gonna
run
around
and
desert
you
one two three
out
EOF

../profsh test.sh >test.out 2>test.err #|| exit

diff -u test.exp test.out || exit

) || exit

rm -fr "$tmp"
