#! /bin/sh

# UCLA CS 111 Lab 1 - Test that syntax errors are caught.

# Copyright 2012-2014 Paul Eggert.

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
status=

# Sanity check, to make sure it works with at least one good example.
echo x >test0.sh || exit
../profsh -t test0.sh >test0.out 2>test0.err || exit
echo '# 1
  x' >test0.exp || exit
diff -u test0.exp test0.out || exit
test ! -s test0.err || {
  cat test0.err
  exit 1
}

#if NAC; then ls ; fi apparently this exits 0 in bash
n=1
echo I am in.txt >in.txt
for bad in \
  'NOT_A_COMMAND' \
  'cat < NOT_A_FILE' \
  'NOT_A_COMMAND | grep s' \
  'ls | NOT_A_COMMAND' \
  'ls ; NOT_A_COMMAND' \
  'NOT_A_COMMAND ; NOT_A_COMMAND' \
  'NAC ; NAC ; NAC' \
  'NAC | NAC' \
  'NAC | NAC | NAC' \
  'NAC | NAC ; NAC' \
  'NAC ;' \
  'if ls ; then NAC ; fi' \
  '(NAC)' \
  '(ls) | NAC' \
  '(cat) <NAF' \
  'ls | grep s <NAF' \
  'if test 1 -eq ; then cat ; fi <NAF' \
  'if true ; then ( cat ) ; fi < NAF' \
  'until false; do cat ; done < NAF' \
  'NAC>out.txt' \
  'NAC <in.txt >out.txt' \
  'if cat<in.txt >out.txt; then NAF ; fi'\
  '(NAC)>out.txt'
do
  echo "$bad" >test$n.sh || exit
  ../profsh  test$n.sh >test$n.out 2>test$n.err && {
    echo >&2 "test$n: unexpectedly succeeded for: $bad"
    status=1
  }
  test -s test$n.err || {
    echo >&2 "test$n: no error message for: $bad"
    status=1
  }
  if (echo $bad | grep out.txt >/dev/null)
  then
  	if !(ls | grep out.txt >/dev/null)
	then
  		echo >&2 "test$n: no output file for: $bad"
		status=1
	fi
  	rm out.txt
  fi

  n=$((n+1))
done

exit $status
) || exit

rm -fr "$tmp"


