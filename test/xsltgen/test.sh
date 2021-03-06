#!/bin/sh -

. `dirname $0`/../xevxml_vars.sh

SRC2XML="src2xml -U1"
XML2SRC=xml2src
XSLTGEN=xsltgen
XSLTEXEC="xsltexec -L $XEVTRANSLIB"

if [ $# -ne 2 ]; then
    echo "USAGE: test.sh test.f90 test.json"
    exit 1
fi

SRC=$1
JSON=$(basename $2)
echo -n "Testing " 1>&2
echo -n "$(basename $SRC) with $JSON" 1>&2

case $SRC in
    *.f90)
	DIR=`echo $SRC |sed 's/.f90$/.f90.xev.d/'`
        EXP=`echo $JSON|sed 's/.json$/.f90/'`
	;;
    *.c)
	DIR=`echo $SRC |sed 's/.c$/.c.xev.d/'`
        EXP=`echo $JSON|sed 's/.json$/.c/'`
	;;
    *.F)
	DIR=`echo $SRC |sed 's/.F$/.F.xev.d/'`
        EXP=`echo $JSON|sed 's/.json$/.F/'`
	;;
esac

XML=`echo $JSON |sed 's/\.json$/\.xml/'`
DIF=`echo $JSON |sed 's/\.json$/\.dif/'`
XSLT=`echo $JSON |sed 's/\.json$/\.xsl/'`
LOG=`echo $JSON |sed 's/.json$/.log/'`

OUT=$DIR

cat /dev/null > $OUT/$LOG
$SRC2XML $SRC 1> $OUT/$XML 2>> $OUT/$LOG
if [ $? -ne 0 ]; then
    echo " NG (src2xml failed)" 1>&2
    exit 1
fi

$XSLTGEN < $DIR/$JSON 1> $OUT/$XSLT 2>> $OUT/$LOG
if [ $? -ne 0 ]; then
    echo " NG (xsltgen failed)" 1>&2
    exit 2
fi


$XSLTEXEC $OUT/$XSLT < $OUT/$XML 1> $OUT/AFTER_$XML 2>> $OUT/$LOG
if [ $? -ne 0 ]; then
    echo " NG (xsltexec failed)" 1>&2
    exit 3
fi


$XML2SRC < $OUT/AFTER_$XML 1> $OUT/AFTER_$EXP 2>> $OUT/$LOG
if [ $? -ne 0 ]; then
    echo " NG (xml2src failed)" 1>&2
    exit 4
fi

US="_"
diff $OUT/AFTER_$EXP $OUT/EXPECTED_$EXP > $OUT/$DIF
if [ $? -ne 0 -o -s $OUT/$DIF ]; then
    echo " NG (mismatch)" 1>&2
    exit 5
fi

echo " OK" 1>&2
exit 0
