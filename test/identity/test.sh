#!/bin/sh -
SHELLDIR=`dirname $0`

. $SHELLDIR/../xevxml_vars.sh
OUT=$SHELLDIR/output
IN=$SHELLDIR/input

FC=identityTranslator
SRC2XML="src2xml --check_fortran_pragma=false"
XML2SRC=xml2src
ROSECOMP=$FC

if [ $# -ne 1 ]; then
    echo "USAGE: test.sh test.f90"
    exit 1
fi

ABS_SRC=$1
SRC=$(basename $ABS_SRC)
echo -n "Testing \"" 1>&2
echo -n $SRC 1>&2
echo -n "\"" 1>&2

SRCLEN=`expr length $SRC`
COLPOS=`expr 50 - $SRCLEN`
for i in `seq $COLPOS`; do echo -n ' ' 1>&2 ;done
echo -n " r" 1>&2

case $SRC in
    *.f90)
	XML=`echo $SRC |sed 's/.f90$/.xml/'`
	DIF=`echo $SRC |sed 's/.f90$/.dif/'`
	LOG=`echo $SRC |sed 's/.f90$/.log/'`
	;;
    *.c)
	XML=`echo $SRC |sed 's/.c$/.xml/'`
	DIF=`echo $SRC |sed 's/.c$/.dif/'`
	LOG=`echo $SRC |sed 's/.c$/.log/'`
	;;
    *.F)
	XML=`echo $SRC |sed 's/.F$/.xml/'`
	DIF=`echo $SRC |sed 's/.F$/.dif/'`
	LOG=`echo $SRC |sed 's/.F$/.log/'`
	;;
    *.f)
	XML=`echo $SRC |sed 's/.f$/.xml/'`
	DIF=`echo $SRC |sed 's/.f$/.dif/'`
	LOG=`echo $SRC |sed 's/.f$/.log/'`
	;;
    *.f03)
	XML=`echo $SRC |sed 's/.f03$/.xml/'`
	DIF=`echo $SRC |sed 's/.f03$/.dif/'`
	LOG=`echo $SRC |sed 's/.f03$/.log/'`
	;;
esac

# error messages are written in 1
(trap '' 1 2 3 4 5 6 15;\
$FC -rose:skip_syntax_check -rose:skipfinalCompileStep -c $ABS_SRC -rose:o $OUT/rose_$SRC 2> $OUT/$LOG ) 2> /dev/null
if [ $? -ne 0 ]
then
    echo -n "   " 1>&2
    echo " NG (terminated)" 1>&2
    exit 1
fi

echo -n "s" 1>&2
$SRC2XML $ABS_SRC 1> $OUT/$XML 2>> $OUT/$LOG
if [ $? -ne 0 ]
then
    echo -n "  " 1>&2
    echo " NG (terminated)" 1>&2
    exit 1
fi

echo -n "x" 1>&2
$XML2SRC $OUT/xev_$SRC < $OUT/$XML  2>> $OUT/$LOG
if [ $? -ne 0 ]
then
    echo -n " " 1>&2
    echo " NG (terminated)" 1>&2
    exit 1
fi

echo -n "d" 1>&2
diff $OUT/rose_$SRC $OUT/xev_$SRC > $OUT/$DIF
if [ $? -eq 0 ];
then
    echo -n " OK" 1>&2
else
    echo " NG (mismatch)" 1>&2
    exit 1
fi

if [ -s $OUT/$LOG ]
then
    echo " (warning)" 1>&2
else
    echo " " 1>&2
fi

exit 0
